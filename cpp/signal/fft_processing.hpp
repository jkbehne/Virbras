/**
 * This file houses basic tools for real(ish) time FFT processing using overlapp-add techniques
 * for filtering. In the future, it may also house functions for running more advanced FFT processing
 * including overlap-add processing with windows and STFT manipulations.
*/
#pragma once

#include <bit>

#include <eigen3/Eigen/Dense>
#include <fftw3.h>

#include "signal/sample_rate_firs.hpp"

namespace Signal {

/**
 * Simple class to manage real-time convolution in the frequency domain. Transients
 * are handled via overlap-add.
*/
template<typename ScalarType>
class OverlapAddConvolver
{
  public: // Members
    const unsigned int window_size;
    const unsigned int num_transients;
    const unsigned int output_size;

  private: // Members
    DFTConvolver<ScalarType> convolver;
    std::vector<ScalarType> output;

    unsigned int write_idx;

    unsigned int input_idx;
    unsigned int output_idx;

  private: // Methods
    /**
     * Writes zeros to all outputs that need to be overridden
    */
    void ready_output()
    {
      // Figure out where to write zeros
      const unsigned int num_zeros = window_size;
      const unsigned int zero_start = (write_idx + num_transients) % output_size;

      // Write the zeros
      #pragma omp parallel for
      for (unsigned int i = 0; i < num_zeros; ++i)
      {
        const unsigned int idx = (zero_start + i) % output_size;
        output[idx] = {};
      }
    }

    /**
     * Writes outputs via addition (so transients sum to full filter)
    */
    void write_output()
    {
      // Write the output from the convolver
      #pragma omp parallel for
      for (unsigned int i = 0; i < output_size; ++i)
      {
        const unsigned int idx = (write_idx + i) % output_size;
        output[idx] += convolver.output[idx];
      }

      // Update the indexes
      output_idx = write_idx;
      write_idx = (write_idx + window_size) % output_size;
    }

  public: // Methods
    /**
     * Construct the object from a window size (input data) and filter
    */
    OverlapAddConvolver(
      const unsigned int window_size_,
      const std::vector<ScalarType>& filter_coeffs
    ) : window_size(window_size_),
        convolver({window_size, filter_coeffs}),
        num_transients(convolver.filter_size - 1),
        output_size(convolver.output_size),
        write_idx(),
        input_idx(),
        output_idx() {}

    /**
     * Push input to buffer, do FFT (if ready), run filter, output result
    */
    ScalarType next(const ScalarType x)
    {
      // Push the input into the buffer and check if we need to do an FFT
      convolver.input[input_idx] = static_cast<double>(x);
      ++input_idx;
      if (input_idx == window_size)
      {
        // Reset input idx and run the filter in the frequency domain
        input_idx = 0;
        convolver.run_filter();

        // Do overlap-add on the output
        ready_output();
        write_output();
      }

      const auto out = output[output_idx];
      ++output_idx;
      output_idx = output_idx % output_size;
      return out;
    }
};

/**
 * Engine for running convolutions in the frequency domain. Handles zero padding for transients
 * and uses FFT powers of two to do so (other FFT lengths could be evaluated in the future).
*/
template<typename ScalarType>
class DFTConvolver
{
  public: // Members
    const unsigned int input_size;
    const unsigned int filter_size;
    const unsigned int output_size;
    const unsigned int num_fft;

    double* input;
    double* output;

  private: // Members
    fftw_complex* input_freq;
    fftw_complex* filter_freq;

    fftw_plan fwd_plan;
    fftw_plan inv_plan;

  public: // Methods
    /**
     * Construct the object from an input size and filter coefficients
     * 
     * Order of operations here:
     * 1.) Allocate memory of the appropriate types and sizes
     * 2.) Copy the filter into the input
     * 3.) Run the FFT for the filter
     * 4.) Clean-up the filter FFT
     * 5.) Set the input back to zeros
     * 6.) Setup remaining FFT plans
    */
    DFTConvolver(
      const unsigned int input_size_,
      const std::vector<ScalarType>& filter_coeffs
    ) : input_size(input_size_),
        filter_size(filter_coeffs.size()),
        output_size(input_size + filter_size - 1),
        num_fft(std::bit_ceil(output_size))
    {
      // Allocate memory
      input = new double[num_fft]();
      output = new double[num_fft]();
      input_freq = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * num_fft);
      filter_freq = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * num_fft);

      // Setup FFT plan for the filter
      fftw_plan filter_plan = fftw_plan_dft_r2c_1d(num_fft, input, filter_freq, FFTW_ESTIMATE);

      // Copy filter coefficients
      for (unsigned int i = 0; i < filter_coeffs.size(); ++i)
      {
        input[i] = static_cast<double>(filter_coeffs[i]);
      }

      // Run the FFT for the filter
      fftw_execute(filter_plan);
      fftw_destroy_plan(filter_plan);

      // Set the input to zero again
      for (unsigned int i = 0; i < filter_coeffs.size(); ++i)
      {
        input[i] = {};
      }

      // Make the other plans
      fwd_plan = fftw_plan_dft_r2c_1d(num_fft, input, input_freq, FFTW_MEASURE);
      inv_plan = fftw_plan_dft_c2r_1d(num_fft, input_freq, output, FFTW_MEASURE);
    }

    /**
     * Runs the filter in the frequency domain
     * 
     * Note: Frequency domain multiply will run in parallel
     * TODO: Check caching behavior of frequency domain multiply
    */
    void run_filter()
    {
      fftw_execute(fwd_plan);

      #pragma omp parallel for
      for (unsigned int i = 0; i < num_fft; ++i)
      {
        const double a = input_freq[i][0];
        const double b = input_freq[i][1];
        const double c = filter_freq[i][0];
        const double d = filter_freq[i][1];
        input_freq[i][0] = a * c - b * d;
        input_freq[i][1] = a * d + b * c;
      }

      fftw_execute(inv_plan);
    }

    /**
     * Destructor frees array memory and destorys FFT plans
    */
    ~DFTConvolver()
    {
      delete[] input;
      delete[] output;
      fftw_free(input_freq);
      fftw_free(filter_freq);
      fftw_destroy_plan(fwd_plan);
      fftw_destroy_plan(inv_plan);
    }
};

/**
 * Paired intepolation and decimation operation engine
*/
template<typename ScalarType, int UpFactor>
class PairedInterpolatorDecimator
{
  // TODO: Consider designing other filters so this can be changed
  static_assert(UpFactor == 4, "Up-sampling of 4x is the only supported level");

  public: // Types
    typedef Eigen::Matrix<ScalarType, UpFactor, 1> InterpType;

  public: // Static Members
    static constexpr ScalarType scale = static_cast<ScalarType>(UpFactor);
    static const std::vector<ScalarType> filter = FIR::array_to_vector<ScalarType>(FIR::interp_769_4x, 769);

  private: // Members
    OverlapAddConvolver<ScalarType> convolver;

  public: // Methods
    explicit PairedInterpolatorDecimator(const unsigned int window_size_) : convolver({window_size_, filter}) {}

    InterpType interpolate(const ScalarType xn)
    {
      InterpType out;
      for (unsigned int i = 0; i < UpFactor; ++i)
      {
        out(i) = scale * convolver.next(i == 0 ? xn : {});
      }
      return out;
    }

    ScalarType decimate(const InterpType& input)
    {
      ScalarType out;
      for (unsigned int i = 0; i < UpFactor; ++i)
      {
        if (i == UpFactor - 1) out = convolver.next(input(i));
        else convolver.next(input(i));
      }
      return out;
    }
};

} // End namespace Signal
