#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "AudioFile.h"

/**
 * Implements a simple, read-only stream of scalar numeric data.
 * This class intends to unify both numeric data that has already been
 * streamed into a buffer (such as a std::vector) and data that is being
 * streamed in real time.
*/
template<typename ScalarType>
class InputSignalStream
{
public: // Methods
  /**
   * Return optional scalar type (no value means stream is done)
  */
  virtual const std::optional<ScalarType> read_next() = 0;
};

template<typename ScalarType>
struct InputBase
{
  public: // Types
    typedef InputSignalStream<ScalarType> type;
    typedef std::shared_ptr<InputSignalStream<ScalarType>> ptr_type;
};

/**
 * Implements a simple, write-only stream of scalar numeric data.
 * This class intends to unify both numeric data that is being streamed
 * into a buffer of known size (if the input size is known) and data that
 * is being streamed to some output in real time.
*/
template<typename ScalarType>
class OutputSignalStream
{
public: // Methods
  /**
   * Write the next value in the output signal
  */
  virtual void write_next(const ScalarType) = 0;
};

template<typename ScalarType>
struct OutputBase
{
  public: // Types
    typedef OutputSignalStream<ScalarType> type;
    typedef std::shared_ptr<OutputSignalStream<ScalarType>> ptr_type;
};

/**
 * Adapts a std::vector input to an InputSignalStream
*/
template<typename ScalarType>
class STLVectorInputSignal : public InputSignalStream<ScalarType>
{
public: // Members
  std::vector<ScalarType> input;
  typename std::vector<ScalarType>::iterator iterator;

public: // Methods
  STLVectorInputSignal(std::vector<ScalarType> input_) : input(std::move(input_))
  {
    iterator = input.begin();
  }

  virtual const std::optional<ScalarType> read_next() override
  {
    if (iterator == input.end()) return {};
    return *(iterator++);
  }
};

/**
 * Adapts a std::vector output to an OutputSignalStream
*/
template<typename ScalarType>
class STLVectorOutputSignal : public OutputSignalStream<ScalarType>
{
public: // Members
  std::vector<ScalarType> output;

public: // Methods
  STLVectorOutputSignal() : output(std::vector<ScalarType>()) {}
  STLVectorOutputSignal(const int signal_size) : output(std::vector<ScalarType>())
  {
    output.reserve(signal_size);
  }

  virtual void write_next(const ScalarType value) override
  {
    output.push_back(value);
  }
};

template<typename ScalarType>
class SingleChannelOwned : public InputSignalStream<ScalarType>
{
  private: // Members
    int buffer_idx;
    const std::vector<ScalarType>& buffer;

  public: // Methods
    SingleChannelOwned(
      const std::vector<ScalarType>& buffer_
    ) : buffer_idx(0), buffer(buffer_) {}

    virtual const std::optional<ScalarType> read_next() override
    {
      if (buffer_idx >= buffer.size()) return {};
      const auto output = buffer[buffer_idx];
      ++buffer_idx;
      return output;
    }
};

template<typename ScalarType>
class SingleOutputOwned : public OutputSignalStream<ScalarType>
{
  private: // Members
    int buffer_idx;
    std::vector<ScalarType>& buffer;

  public: // Methods
    SingleOutputOwned(std::vector<ScalarType>& buffer_) : buffer_idx(0), buffer(buffer_) {}

    virtual void write_next(const ScalarType output) override
    {
      assert(buffer_idx < buffer.size());
      buffer[buffer_idx] = output;
      ++buffer_idx;
    }
};

template<typename ScalarType>
class DualChannelFileInput
{
  public: // Types
    typedef typename InputBase<ScalarType>::ptr_type InputBasePtr;

  public: // Members
    AudioFile<ScalarType> audio_file;
    InputBasePtr left_channel;
    InputBasePtr right_channel;

  public: // Methods
    DualChannelFileInput(const std::string& fname, const bool verbose = false)
    {
      // Load the audio file
      audio_file.load(fname);
      assert(audio_file.getNumChannels() == 2); // Expecting two channels
      if (verbose) audio_file.printSummary();

      // Create the channels
      left_channel = std::make_shared<SingleChannelOwned<ScalarType>>(audio_file.samples[0]);
      right_channel = std::make_shared<SingleChannelOwned<ScalarType>>(audio_file.samples[1]);
    }
};

template<typename ScalarType>
class DualChannelFileOutput
{
  public: // Types
    typedef typename OutputBase<ScalarType>::ptr_type OutputBasePtr;

  public: // Members
    AudioFile<ScalarType> audio_file;
    typename AudioFile<ScalarType>::AudioBuffer buffer;
    OutputBasePtr left_out;
    OutputBasePtr right_out;

  public: // Methods
    DualChannelFileOutput(
      const int sample_rate,
      const int bit_depth,
      const int num_output_samples
    )
    {
      audio_file.setNumChannels(2);
      audio_file.setSampleRate(sample_rate);
      audio_file.setBitDepth(bit_depth);

      buffer.resize(2);
      buffer[0].resize(num_output_samples);
      buffer[1].resize(num_output_samples);

      left_out = std::make_shared<SingleOutputOwned<ScalarType>>(buffer[0]);
      right_out = std::make_shared<SingleOutputOwned<ScalarType>>(buffer[1]);
    }

    void write_final(const std::string& ofile)
    {
      const auto set_success = audio_file.setAudioBuffer(buffer);
      assert(set_success);
      const auto save_success = audio_file.save(ofile);
      assert(save_success);
    }
};
