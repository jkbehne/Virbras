/**
 * The intent of this file is to implement a basic version of the "freeverb" artificial
 * reverberation algorithm. The particular specification implemented here comes from
 * Chapter 3.6 of Physical Audio Signal Processing by Julius O. Smith III, see:
 * 
 * https://ccrma.stanford.edu/~jos/pasp/
 * 
 * for the online version of the book.
*/
#pragma once

#include "signal/combiners.hpp"

/**
 * Make a "freeverb" filter using some default specs
 * 
 * The overall output is:
 * 
 * [outputL outputR]^T = dry * [inputL inputR]^T + [[wet1 wet2]^T [wet2 wet1]^T] [outL outR]^T
 * 
 * where outL and outR are the outputs of the left input through the left filters and the
 * right input through the right filters respectively. In this version, the only difference
 * between the left and right filters is that the delays in the right filters have an
 * additional stereo_spread factor added to them. For more details on the interpretations
 * of the various parameters, see the source cited in the file comment.
*/
template<typename ScalarType>
MIMOIIRFilter<ScalarType> make_freeverb_filter(
  const int stereo_spread,
  const ScalarType dry,
  const ScalarType wet1,
  const ScalarType wet2,
  const ScalarType damp,
  const ScalarType reflect,
  const ScalarType g
)
{
  // Define needed constants
  const int lbcf_delay1 = 1557;
  const int lbcf_delay2 = 1617;
  const int lbcf_delay3 = 1491;
  const int lbcf_delay4 = 1422;
  const int lbcf_delay5 = 1277;
  const int lbcf_delay6 = 1356;
  const int lbcf_delay7 = 1188;
  const int lbcf_delay8 = 1116;

  const int ap_delay1 = 225;
  const int ap_delay2 = 556;
  const int ap_delay3 = 441;
  const int ap_delay4 = 341;

  typedef typename FilterVariant<ScalarType>::type FilterVariantType;

  // Create the parallel lowpass-feedback comb filters (left channel)
  const ScalarType alpha = reflect * (1.0 - damp);
  const ScalarType beta = damp;
  std::vector<FilterVariantType> left_lbcf_filters {
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay1)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay2)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay3)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay4)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay5)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay6)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay7)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay8))
  };
  ParallelCombination<ScalarType> left_lbfc_bank(
    left_lbcf_filters, Eigen::Matrix<ScalarType, Eigen::Dynamic, 1>::Ones(8)
  );

  // Create the series all-pass approximation filters (left channel)
  SeriesCombination<ScalarType> left_ap_series(
    std::vector<FilterVariantType> {
      FilterVariantType(FeedforwardFeedbackCombFilter(-1.0, 1.0 + g, g, ap_delay1)),
      FilterVariantType(FeedforwardFeedbackCombFilter(-1.0, 1.0 + g, g, ap_delay2)),
      FilterVariantType(FeedforwardFeedbackCombFilter(-1.0, 1.0 + g, g, ap_delay3)),
      FilterVariantType(FeedforwardFeedbackCombFilter(-1.0, 1.0 + g, g, ap_delay4))
    }
  );

  // Create the parallel lowpass-feedback comb filters (right channel)
  std::vector<FilterVariantType> right_lbcf_filters {
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay1 + stereo_spread)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay2 + stereo_spread)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay3 + stereo_spread)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay4 + stereo_spread)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay5 + stereo_spread)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay6 + stereo_spread)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay7 + stereo_spread)),
    FilterVariantType(FilteredFeedbackCombFilter(alpha, beta, lbcf_delay8 + stereo_spread))
  };
  ParallelCombination<ScalarType> right_lbfc_bank(
    right_lbcf_filters, Eigen::Matrix<ScalarType, Eigen::Dynamic, 1>::Ones(8)
  );

  // Create the series all-pass approximation filters (right channel)
  SeriesCombination<ScalarType> right_ap_series(
    std::vector<FilterVariantType> {
      FilterVariantType(FeedforwardFeedbackCombFilter(-1.0, 1.0 + g, g, ap_delay1 + stereo_spread)),
      FilterVariantType(FeedforwardFeedbackCombFilter(-1.0, 1.0 + g, g, ap_delay2 + stereo_spread)),
      FilterVariantType(FeedforwardFeedbackCombFilter(-1.0, 1.0 + g, g, ap_delay3 + stereo_spread)),
      FilterVariantType(FeedforwardFeedbackCombFilter(-1.0, 1.0 + g, g, ap_delay4 + stereo_spread))
    }
  );

  // Create the final filter structure
  Eigen::Matrix<ScalarType, 2, 2> output_lt {{wet1, wet2}, {wet2, wet1}};
  return MIMOIIRFilter<ScalarType>(
    dry,
    output_lt,
    {
      std::make_shared<SeriesCombination<ScalarType>>(
        std::vector<FilterVariantType> {
          FilterVariantType(left_lbfc_bank),
          FilterVariantType(left_ap_series)
        }
      ),
      std::make_shared<SeriesCombination<ScalarType>>(
        std::vector<FilterVariantType> {
          FilterVariantType(right_lbfc_bank),
          FilterVariantType(right_ap_series)
        }
      )
    }
  );
}
