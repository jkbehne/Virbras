/**
 * The intent of this file is to provide a template library for so
 * called tapped delay lines. Essentially, these systems implement
 * finite difference equations:
 * 
 * y[n] = sum_(k=1:N) b_k * x[n - M_k]
 * 
 * where N is the number of taps and M_k is the delay in integer
 * samples for tap k. By convention, we assume M_1 = 0, so there's
 * always one tap corresponding to the input signal. Note, if that's
 * not desired, b_1 can always be set to zero.
 * 
 * While this library is generically templated, it is obviously only
 * reasonable for integer and floating point scalar types. Anything else
 * will likely produce strange results, if it even compiles.
*/
#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "signal/filter_base.hpp"

/**
 * Basic class for tapped delay line templated on the scalar type
 * 
 * Note: The scalar type corresponds to the type of each entry in the
 * input and output. For this use case, those must match.
 * 
 * This class implements the finite difference equation described in the
 * file comment. Depending on the input and output streams, it can operate
 * on real-time data sequences. Though note, it can also be setup to operate
 * on non-real-time sequence as well (this is done for testing).
*/
template<typename ScalarType>
class TappedDelayLine : public FIRFilter<ScalarType>
{
  public: // Members
    const std::vector<int> delays;
    const std::vector<ScalarType> coeffs;

  private: // Members
    std::vector<ScalarType> buffer;
    int buffer_idx;

  public: // Methods
    /**
     * Produce the next output based on the current input and internal state
     * 
     * A circular buffer is used to manage reading the delayed samples.
    */
    virtual ScalarType next(const ScalarType input) override
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

    virtual const int max_delay() const override
    {
      return buffer.size();
    }

    /**
     * Constructor mostly deals with finding max delay and setting up the buffer
     * 
     * Note: The buffer size corresponds with the maximum delay required
    */
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
};
