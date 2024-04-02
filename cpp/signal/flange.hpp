#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <optional>
#include <vector>

#include "signal/time_varying_delay.hpp"

/**
 * Implements a sine function intended for time-varying delay in a flanger
 * 
 * The basic equation is:
 * 
 * m[n] = average_delay * (1 + sweep * sin(2 * pi * speed * n * sample_period))
 * 
 * where n is the index of the current sample and hence m[n] will be the delay
 * requested at time n for the overall effect function
*/
struct SinusoidalDelay
{
  public: // Members
    const double average_delay;
    const double sweep;
    const double speed; // Frequency
    const double sample_period;

  private: // Members
    int delay_idx;

  public: // Methods
    /**
     * Construct the object from average_delay, sweep, speed, and sample_period
     * 
     * Asserts that average_delay > 0
     * Asserts that speed > 0
     * Asserts that sample_period > 0
     * Asserts that sweep is on [-1, 1]
    */
    SinusoidalDelay(
      const double average_delay_,
      const double sweep_,
      const double speed_,
      const double sample_period_,
    ) : average_delay(average_delay_),
        sweep(sweep_),
        speed(speed_),
        sample_period(sample_period_),
        delay_idx(0)
    {
      // Assert that the configuration is reasonable
      assert(average_delay > 0.0);
      assert(speed > 0.0);
      assert(sample_period > 0.0);
      assert(-1.0 <= sweep and sweep <= 1.0);
    }

    /**
     * Call operator for getting the next delay value
    */
    double operator()()
    {
      const double n = static_cast<double>(delay_idx);
      const auto delay = average_delay * (1.0 + sweep * sin(2.0 * M_PI * speed * n * sample_period));
      ++delay_idx;
      return delay;
    }

    /**
     * Function to compute the maximum delay that can be given
    */
    int max_delay() const
    {
      return static_cast<int>(ceil(average_delay * (1.0 + sweep)));
    }
};

/**
 * Basic single-input single-output flanger
 * 
 * Inverting the phase negates the coefficient supplied by depth
*/
template<typename ScalarType, typename DelayType>
class Flanger
{
  public: // Types
    typedef std::optional<ScalarType> ReadType;

  public: // Members
    const ScalarType depth;
    const bool invert_phase;
    DelayType delay;

  private: // Members
    TimeVaryingDelayLine<ScalarType> delay_line;

  public: // Methods
    Flanger(
      const ScalarType depth_,
      const bool invert_phase_,
      DelayType delay_
    ) : depth(depth_),
        invert_phase(invert_phase_),
        delay(std::move(delay_)),
        delay_line(
          {
            this->delay.max_delay(),
            1.0 /* input coeff */,
            this->invert_phase ? -1.0 * this->depth : this->depth
          }
        )
        {
          assert(0.0 <= depth and depth <= 1.0);
        }

    /**
     * Process the input / output streams with the time-varying delay
    */
    void process(
      std::function<ReadType()>& in_stream,
      std::function<void(const ScalarType)>& out_stream
    )
    {
      // Call process of the delay line
      delay_line.process(in_stream, out_stream, [&delay](){return delay();});
    }
};

/**
 * Basic flanger effect for a left / right channel input / output setup
*/
template<typename ScalarType, typename DelayType>
class TwoChannelFlanger
{
  public: // Members
    const std::vector<ScalarType>& l_read;
    const std::vector<ScalarType>& r_read;

    Flanger<ScalarType, DelayType> l_flanger;
    Flanger<ScalarType, DelayType> r_flanger;

  private: // Members
    std::vector<ScalarType>& l_write;
    std::vector<ScalarType>& r_write;

  public: // Methods
    TwoChannelFlanger(
      const std::vector<ScalarType>& l_read_,
      const std::vector<ScalarType>& r_read_,
      Flanger<ScalarType, DelayType> l_flanger_,
      Flanger<ScalarType, DelayType> r_flanger_,
      std::vector<ScalarType>& l_write_,
      std::vector<ScalarType>& r_write_
    ) : l_read(l_read_),
        r_read(r_read_),
        l_flanger(std::move(l_flanger_)),
        r_flanger(std::move(r_flanger_)),
        l_write(l_write_),
        r_write(r_write_)
    {
      assert(l_read.size() == r_read.size());
      assert(l_write.size() == r_write.size());
      assert(l_read.size() <= l_write.size());
    }

    // TODO: This function is definitely too long...
    void process(const std::optional<ScalarType> rescale_abs = {})
    {
      // Do the left channel first
      int l_read_idx = 0;
      auto l_read_fn = [&l_read, &l_read_idx]()
      {
        const auto read = l_read[l_read_idx];
        ++l_read_idx;
        return read;
      };
      int l_write_idx = 0;
      auto l_write_fn = [&l_write, &l_write_idx](const ScalarType output)
      {
        l_write[l_write_idx] = output;
        ++l_write_idx;
      };
      l_flanger.process(l_read_fn, l_write_fn);

      // Do the right channel next
      int r_read_idx = 0;
      auto r_read_fn = [&r_read, &r_read_idx]()
      {
        const auto read = r_read[r_read_idx];
        ++r_read_idx;
        return read;
      };
      int r_write_idx = 0;
      auto r_write_fn = [&r_write, &r_write_idx](const ScalarType output)
      {
        r_write[r_write_idx] = output;
        ++r_write_idx;
      };
      r_flanger.process(r_read_fn, r_write_fn);

      // Do the rescaling if a value is provided
      if (rescale_abs)
      {
        const auto alpha = *rescale_abs;
        assert(alpha > 0.0 and alpha <= 1.0);

        // Get the min / max vals for the left channel
        const auto l_max_iter = std::max_element(l_write.begin(), l_write.end());
        assert(l_max_iter != l_write.end()); // Make sure we found a maximum
        const auto l_min_iter = std::min_element(l_write.begin(), l_write.end());
        assert(l_min_iter != l_write.end()); // Make sure we found a minimum

        const auto l_max_val = *l_max_iter;
        const auto l_min_val = *l_min_iter;

        // Compute shifting / scaling factors for left channel
        const auto l_max_m_min = l_max_val - l_min_val;
        assert(l_max_m_min != 0.0);
        const auto l_scale = 2.0 * alpha / l_max_m_min;
        const auto l_shift = -1.0 * alpha * (l_max_val + l_min_val) / l_max_m_min;

        // Do the actual transform for left channel
        #pragma omp parallel for
        for (int i = 0; i < l_write.size(); ++i)
        {
          const auto val = l_write[i];
          l_write[i] = l_scale * val + l_shift;
        }

        // Get the min / max vals for the right channel
        const auto r_max_iter = std::max_element(r_write.begin(), r_write.end());
        assert(r_max_iter != r_write.end()); // Make sure we found a maximum
        const auto r_min_iter = std::min_element(r_write.begin(), r_write.end());
        assert(r_min_iter != r_write.end()); // Make sure we found a minimum

        const auto r_max_val = *r_max_iter;
        const auto r_min_val = *r_min_iter;

        // Compute shifting / scaling factors for right channel
        const auto r_max_m_min = r_max_val - r_min_val;
        assert(r_max_m_min != 0.0);
        const auto r_scale = 2.0 * alpha / r_max_m_min;
        const auto r_shift = -1.0 * alpha * (r_max_val + r_min_val) / r_max_m_min;

        // Do the actual transform for right channel
        #pragma omp parallel for
        for (int i = 0; i < r_write.size(); ++i)
        {
          const auto val = r_write[i];
          r_write[i] = r_scale * val + r_shift;
        }
      }
    }
};
