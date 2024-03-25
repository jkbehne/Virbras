#pragma once

#include <iostream>

#include <algorithm>
#include <memory>
#include <vector>

#include "signal/signal_stream.hpp"

template<typename ScalarType>
class TappedDelayLine
{
  public: // Types
    typedef std::shared_ptr<InputSignalStream<ScalarType>> InputType;
    typedef std::shared_ptr<OutputSignalStream<ScalarType>> OutputType;

  public: // Members
    const std::vector<int> delays;
    const std::vector<ScalarType> coeffs;

  private: // Members
    std::vector<ScalarType> buffer;
    int buffer_idx;

  private: // Methods
    ScalarType next(const ScalarType input)
    {
      // Compute the output
      auto output = coeffs.front() * input;
      for (int i = 0; i < delays.size(); ++i)
      {
        // Grab the things we need
        const auto delay = delays[i];
        const auto coeff = coeffs[i + 1];

        // Compute the index for the current delay
        auto idx = buffer_idx - delay;
        if (idx < 0) idx += buffer.size();

        // Update the overall result
        output += coeff * buffer[idx];
      }

      // Update the buffer
      buffer[buffer_idx++] = input;
      buffer_idx = buffer_idx % buffer.size();

      // Return the previously computed result
      return output;
    }

  public: // Methods
    TappedDelayLine(
      std::vector<int> delays_,
      std::vector<ScalarType> coeffs_
    ) : delays(std::move(delays_)), coeffs(std::move(coeffs_)), buffer_idx(0)
    {
      // Check that our inputs are the right size
      assert(coeffs.size() == delays.size() + 1);

      // Determine buffer size and create it
      const auto max_iter = std::max_element(delays.begin(), delays.end());
      int max_delay = 0;
      if (max_iter != delays.end()) max_delay = *max_iter;
      buffer = std::vector<ScalarType>(max_delay);
    }

    void process(const InputType& isignal, const OutputType& osignal)
    {
      // Make sure pointers are valid
      assert(isignal != nullptr);
      assert(osignal != nullptr);

      const int max_delay = buffer.size();

      // Read the input source until it's exhasuted
      while (true)
      {
        const auto input = isignal->read_next();
        if (not input) break; // Input stream is done
        osignal->write_next(next(*input));
      }

      // Use zeros as inputs to calculate the final transients
      for (int i = 0; i < max_delay; ++i)
      {
        osignal->write_next(next({}));
      }
    }
};
