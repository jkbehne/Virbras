#pragma once

#include <cmath>
#include <functional>
#include <optional>
#include <vector>

/**
 * The basic idea here is to implement a time-varying delay line
 * 
 * Implements the following finite difference equation:
 * 
 * y[n] = a * x[n] + b * x[n - m[n]]
 * 
 * where m[n] is a time-varying delay sequence. Interpolation between
 * samples is performed with linear interpolation. This choice may not
 * work well for high bandwidth signals at lower sampling rates, but it
 * tends to sound okay for most audio applications.
*/
template<typename ScalarType>
class TimeVaryingDelayLine
{
  public: // Types
    typedef std::optional<ScalarType> ReadType;

  public: // Members
    const int max_delay;
    const ScalarType input_coeff;
    const ScalarType delay_coeff;

  private: // Members
    std::vector<ScalarType> buffer;
    int buffer_idx;

  public: // Methods
    /**
     * Set the maximum delay, input scaling, and delay scaling
    */
    TimeVaryingDelayLine(
      const int max_delay_,
      const ScalarType input_coeff_,
      const ScalarType delay_coeff_
    ) : max_delay(max_delay_),
        input_coeff(input_coeff_),
        delay_coeff(delay_coeff_),
        buffer(max_delay_),
        buffer_idx(0) {}

    /**
     * Get the next output sample based on the input and a (non-integer) delay
    */
    ScalarType next(const ScalarType input, const double delay)
    {
      // Check that the delay can actually be computed
      const int lower_delay = static_cast<int>(floor(delay));
      const int upper_delay = lower_delay + 1;
      assert(lower_delay >= 0);
      assert(upper_delay <= max_delay);

      // Compute the factional delay value
      const double frac_delay = delay - static_cast<double>(lower_delay);

      // Get the newer delay coefficient
      ScalarType newer {};
      if (lower_delay == 0) newer = input;
      else
      {
        auto lower_idx = buffer_idx - lower_delay;
        if (lower_idx < 0) lower_idx += max_delay;
        newer = buffer[lower_idx];
      }

      // Get the older delay coefficient
      auto upper_idx = buffer_idx - upper_delay;
      if (upper_idx < 0) upper_idx += max_delay;
      const auto older = buffer[upper_idx];

      // Get the (linearly) interpolated value
      const auto delay_output = older + frac_delay * (newer - older);

      // Make the overall output
      const auto output = input_coeff * input + delay_coeff * delay_output;

      // Write the current input to the buffer
      buffer[buffer_idx] = input;
      ++buffer_idx;
      buffer_idx = buffer_idx % max_delay;
    }

    /**
     * Process a whole stream of data (real-time or offline)
    */
    void process(
      std::function<ReadType()>& in_stream,
      std::function<void(const ScalarType)>& out_stream,
      std::function<double()>& delay_stream
    )
    {
      assert(in_stream != nullptr);
      assert(out_stream != nullptr);
      assert(delay_stream != nullptr);

      // Compute the output until the input is exhasuted
      while (true)
      {
        const ReadType input = in_stream();
        if (not input) break; // Input stream is done
        out_stream(next(*input, delay_stream()));
      }

      // Compute max_delay transients
      // TODO: Should the delay continue to vary?
      for (int i = 0; i < max_delay; ++i)
      {
        out_stream(next({}, delay_stream()));
      }
    }
};
