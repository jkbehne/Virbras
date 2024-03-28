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

#include "signal/filter_base.hpp"

template<typename ScalarType>
class FeedforwardFeedbackCombFilter : public IIRFilter<ScalarType>
{
  public: // Members
    const ScalarType input_coeff;
    const ScalarType input_delay_coeff;
    const ScalarType output_coeff;
    const int delay; // TODO: Could be a template parameter

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
    virtual ScalarType next(const ScalarType input) override
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
};

/**
 * Basic structure for a one pole lowpass IIR filter
 * 
 * Implements y[n] = alpha * x[n] + beta * y[n - 1]
*/
template<typename ScalarType>
class OnePoleLowpassFilter : public IIRFilter<ScalarType>
{
  public: // Members
    const ScalarType alpha;
    const ScalarType beta;

  private: // Members
    ScalarType prev_output;

  public: // Methods
    /**
     * See class comment for information about construction parameters
     * 
     * Note: We assert that |beta| < 1 for stability
    */
    OnePoleLowpassFilter(
      const ScalarType alpha_,
      const ScalarType beta_
    ) : alpha(alpha_), beta(beta_)
    {
      assert(abs(beta) < 1);
    }

    /**
     * Implements the difference equation given in the class comment for one sample
    */
    virtual ScalarType next(const ScalarType input) override
    {
      const auto output = alpha * input + beta * prev_output;
      prev_output = output;
      return output;
    }
};

/**
 * Structure to implement filtered feedback comb filter
 * 
 * Implements the transfer function:
 * 
 * H(z) = 1 / (1 - H_lp(z) * z^(-m))
 * 
 * where H_lp(z) = alpha / (1 - beta z^(-1))
*/
template<typename ScalarType>
class FilteredFeedbackCombFilter : public IIRFilter<ScalarType>
{
  public: // Members
    const ScalarType alpha;
    const ScalarType beta;
    const int feedback_delay;

  private: // Members
    OnePoleLowpassFilter<ScalarType> lp_filter;
    std::vector<ScalarType> out_buffer;
    int buffer_idx;

  public: // Methods
    /**
     * Construct based on one pole lowpass parameters and a feedback delay
    */
    FilteredFeedbackCombFilter(
      const ScalarType alpha_,
      const ScalarType beta_,
      const int feedback_delay_
    ) : alpha(alpha_),
        beta(beta_),
        feedback_delay(feedback_delay_),
        lp_filter(OnePoleLowpassFilter(alpha, beta)),
        out_buffer(std::vector<ScalarType>(feedback_delay)),
        buffer_idx(0) {}

    /**
     * Implement the transfer function in the class comment for a single sample
    */
    virtual ScalarType next(const ScalarType input) override
    {
      const auto delay_output = out_buffer[buffer_idx];
      const auto output = input + lp_filter.next(delay_output);
      out_buffer[buffer_idx] = output;
      ++buffer_idx;
      return output;
    }
};
