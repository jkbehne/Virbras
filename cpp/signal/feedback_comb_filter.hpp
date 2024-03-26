/**
 * The intent of this file is to provide a template library for a
 * feedback-feedforward comb filter. This implements a finite difference
 * equation of the form:
 * 
 * y[n] = b0 * x[n] + b1 * x[n - m] + a * y[n - m]
 * 
 * where m is an integer-valued delay. Note that stability requires
 * |a| < 1. The class will assert if this is not the case.
 * 
 * While this library is generically templated, it really only makes
 * sense for floating point (real or complex) scalar types. Integers
 * would make sense aside from the requirement on a for stability.
*/
#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include "signal/signal_stream.hpp"

template<typename ScalarType>
class FeedforwardFeedbackCombFilter
{
  public: // Types
    typedef std::shared_ptr<InputSignalStream<ScalarType>> InputType;
    typedef std::shared_ptr<OutputSignalStream<ScalarType>> OutputType;

  public: // Members
    const ScalarType input_coeff;
    const ScalarType input_delay_coeff;
    const ScalarType output_coeff;
    const int delay;

  private: // Members
    std::vector<ScalarType> input_buffer;
    std::vector<ScalarType> output_buffer;
    int buffer_idx;

  public: // Methods
    FeedforwardFeedbackCombFilter(
      const ScalarType input_coeff_,
      const ScalarType input_delay_coeff_,
      const ScalarType output_coeff_,
      const int delay_
    ) : input_coeff(input_coeff_),
        input_delay_coeff(input_delay_coeff_),
        output_coeff(output_coeff_),
        delay(delay_),
        buffer_idx(0)
    {
      // Check that the output coeff will generate a stable filter
      assert(abs(output_coeff) < 1.0);

      // Create the buffer
      input_buffer = std::vector<ScalarType>(delay);
      output_buffer = std::vector<ScalarType>(delay);
    }

    /**
     * Produce the next output based on the current input and internal state
     * 
     * A circular buffer is used to manage reading the delayed samples.
    */
    ScalarType next(const ScalarType input)
    {
      // Compute the output
      const auto delay_input = input_buffer[buffer_idx];
      const auto delay_output = output_buffer[buffer_idx];
      const auto delay_term = input_delay_coeff * delay_input + output_coeff * delay_output;
      const auto output = input_coeff * input + delay_term;

      // Update the buffers
      input_buffer[buffer_idx] = input;
      output_buffer[buffer_idx] = output;
      ++buffer_idx;
      buffer_idx = buffer_idx % delay;

      // Return the previously computed result
      return output;
    }

    /**
     * Process an entire input and write to the output until input is exhausted
     * 
     * Note: This function won't return if the input stream is a real-time
     * stream, unless the stream eventually fails to return a result.
     * 
     * Note: This function writes the number of specified delay transients,
     * although technically this is implementing an IIR filter.
    */
    void process(
      const InputType& isignal,
      const OutputType& osignal,
      const int num_output_transients
    )
    {
      // Make sure pointers are valid
      assert(isignal != nullptr);
      assert(osignal != nullptr);

      // Read the input source until it's exhasuted
      while (true)
      {
        const auto input = isignal->read_next();
        if (not input) break; // Input stream is done
        osignal->write_next(next(*input));
      }

      // Use zeros as inputs to calculate the final transients
      for (int i = 0; i < num_output_transients; ++i)
      {
        osignal->write_next(next({}));
      }
    }
};