/**
 * This file houses aggregating classes for basic tube amplifier approximations
*/
#pragma once

#include <vector>

#include "signal/fft_processing.hpp"
#include "signal/triode.hpp"
#include "signal/utilities.hpp"

namespace Signal {

/**
 * This class implements a simple Class-A guitar pre-amplifier. Note that the tube and EQ
 * models are fairly simple in terms of representing the complexity of a real guitar
 * amplifier. Upsampling is performed in the FFT domain using the overlap-add technique and
 * is intended to avoid aliasing frequencies induced by the non-linear triode modeling
*/
template<typename ScalarType, int UpFactor>
class TubePreAmpClassA
{
  public: // Types
    typedef typename PairedInterpolatorDecimator<ScalarType, UpFactor>::InterpType InterpType;
  public: // Members
    ScalarType input_level;
    ScalarType output_level;

    std::vector<TriodeClassAModel<ScalarType>> pre_eq_triodes;
    std::vector<TriodeClassAModel<ScalarType>> post_eq_triodes;
    FirstOrderFilter<ScalarType> low_shelf_filter;
    FirstOrderFilter<ScalarType> high_shelf_filter;

    PairedInterpolatorDecimator<ScalarType, UpFactor> interp_dec;

  public: // Methods
    /**
     * Constructor
     * 
     * This function asserts that we have at least one pre-EQ triode and one post-EQ triode
     * and that the low and high shelf filters have the correct corresponding enum type
    */
    TubePreAmpClassA(
      const ScalarType input_level_dB,
      const ScalarType output_level_dB,
      std::vector<TriodeClassAModel<ScalarType>> pre_eq_triodes_,
      std::vector<TriodeClassAModel<ScalarType>> post_eq_triodes_,
      const FirstOrderFilter<ScalarType> low_shelf_filter_,
      const FirstOrderFilter<ScalarType> high_shelf_filter_
    ) : input_level(Utils::from_dB(input_level_dB)),
        output_level(Utils::from_dB(output_level_dB)),
        pre_eq_triodes(std::move(pre_eq_triodes_)),
        post_eq_triodes(std::move(post_eq_triodes_)),
        low_shelf_filter(low_shelf_filter_),
        high_shelf_filter(high_shelf_filter_),
        interp_dec(256 /* window size - hardcode for now */)
    {
      // Check we have triodes on both ends
      assert(pre_eq_triodes.size() > 0);
      assert(post_eq_triodes.size() > 0);

      // Check that the filters have the right type
      assert(low_shelf_filter.filter_type == FirstOrderFilterType::LowShelving);
      assert(high_shelf_filter.filter_type == FirstOrderFilterType::HighShelving);
    }

    /**
     * The overarching next function
     * 
     * Basic steps for this function are:
     *   1.) Upsample the signal to avoid aliasing
     *   2.) Run amp modeling at the higher processing rate
     *   3.) Downsample the signal to the original rate after anti-alias filtering
    */
    ScalarType next(const ScalarType xn)
    {
      // Do the interpolation
      const InterpType interp = interp_dec.interpolate(xn);
      InterpType out;
      for (unsigned int i = 0; i < UpFactor; ++i)
      {
        out(i) = single_next(interp(i));
      }

      // Do the decimation and return the result
      return interp_dec.decimate(out);
    }

  private: // Methods
    /**
     * Next function to run a single sample through the amp model
    */
    ScalarType single_next(const ScalarType x)
    {
      // Do the pre-eq triodes
      ScalarType out = input_level * x;
      for (auto iter = pre_eq_triodes.begin(); iter != pre_eq_triodes.end(); ++iter)
      {
        out = iter->next(out);
      }

      // Do the eq
      out = low_shelf_filter.next(out);
      out = high_shelf_filter.next(out);

      // Do the post-eq triodes
      for (auto iter = post_eq_triodes.begin(); iter != post_eq_triodes.end(); ++iter)
      {
        out = iter->next(out);
      }

      return output_level * out;
    }
};
} // End namespace Signal
