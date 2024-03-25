#pragma once

#include <optional>
#include <vector>

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
