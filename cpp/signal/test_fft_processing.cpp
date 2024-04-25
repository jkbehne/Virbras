#define BOOST_TEST_MODULE test_fft_processing
#include <boost/test/included/unit_test.hpp>

#include <cmath>
#include <type_traits>

#include "math/vector_tools.hpp"
#include "signal/analog_to_digital_filters.hpp"
#include "signal/fft_processing.hpp"

using namespace std;
using namespace Signal;

template<typename ScalarType>
void run_dft_convolver_tests()
{
  // For x = [1, 2, 3, 4, 5] and h = [-1, 1, 3] the convolved output is
  // y = [-1, -1, 2, 5, 8, 17, 15]
  // Setup the input, filter, and convolver
  const vector<ScalarType> input {1.0, 2.0, 3.0, 4.0, 5.0};
  const vector<ScalarType> filter {-1.0, 1.0, 3.0};
  DFTConvolver<ScalarType> convolver(input.size(), filter);

  // Copy the input into the convolver
  for (unsigned int i = 0; i < input.size(); ++i)
  {
    convolver.input[i] = static_cast<double>(input[i]);
  }

  // Run the convolution
  convolver.run_filter();

  // Copy output
  vector<ScalarType> out(convolver.output_size);
  for (unsigned int i = 0; i < convolver.output_size; ++i)
  {
    out[i] = static_cast<ScalarType>(convolver.output[i]);
  }
  const vector<ScalarType> expected_out {-1.0, -1.0, 2.0, 5.0, 8.0, 17.0, 15.0};

  // Setup tolerance based on type
  double tolerance;
  if constexpr (is_same_v<ScalarType, float>) tolerance = 1e-6;
  else if constexpr (is_same_v<ScalarType, double>) tolerance = 1e-10;
  else tolerance = 1e-32; // Unexpected type - make tolerance so things are likely to fail

  // Finally check things are equal (within floating point tolerance)
  BOOST_TEST(VectorTools::all_close(out, expected_out, tolerance));
}

template<typename ScalarType>
void run_overlap_add_tests()
{
  // For x = [1, 2, 3, 4, 5] and h = [-1, 1, 3] the convolved output is
  // y = [-1, -1, 2, 5, 8, 17, 15]
  // Setup the input, filter, and convolver
  const vector<ScalarType> input {1.0, 2.0, 3.0, 4.0, 5.0};
  const vector<ScalarType> filter {-1.0, 1.0, 3.0};
  OverlapAddConvolver<ScalarType> convolver(2 /* window_size */, filter);

  // Run filter and define expected output
  const auto out = run_filter(input, convolver, 4 /* num_transients */);
  const vector<ScalarType> expected_out {0.0, -1.0, -1.0, 2.0, 5.0, 8.0, 17.0, 15.0, 0.0};

  // Setup tolerance based on type
  double tolerance;
  if constexpr (is_same_v<ScalarType, float>) tolerance = 1e-6;
  else if constexpr (is_same_v<ScalarType, double>) tolerance = 1e-10;
  else tolerance = 1e-32; // Unexpected type - make tolerance so things are likely to fail

  // Finally check things are equal (within floating point tolerance)
  BOOST_TEST(VectorTools::all_close(out, expected_out, tolerance));

  // Now check that a window size of 3 does the same thing
  OverlapAddConvolver<ScalarType> convolver3(3 /* window_size */, filter);

  // Run filter and define expected output
  const auto out3 = run_filter(input, convolver3, 5 /* num_transients */);
  const vector<ScalarType> expected_out3 {0.0, 0.0, -1.0, -1.0, 2.0, 5.0, 8.0, 17.0, 15.0, 0.0};

  BOOST_TEST(VectorTools::all_close(out3, expected_out3, tolerance));
}

template<typename ScalarType>
void run_interp_dec_tests()
{
  // This just tests that nothing throws an error and such
  const vector<ScalarType> input {1.0, 2.0, 3.0, 4.0, 5.0};
  PairedInterpolatorDecimator<ScalarType, 4> interp_dec(2 /* window size */);

  for (auto iter = input.begin(); iter != input.end(); ++iter)
  {
    const auto interp = interp_dec.interpolate(*iter);
    const auto out = interp_dec.decimate(interp);
    BOOST_TEST(not isnan(out));
  }
}

BOOST_AUTO_TEST_CASE( test_dft_convolver )
{
  run_dft_convolver_tests<float>();
  run_dft_convolver_tests<double>();
}

BOOST_AUTO_TEST_CASE( test_overlap_add )
{
  run_overlap_add_tests<float>();
  run_overlap_add_tests<double>();
}

BOOST_AUTO_TEST_CASE( test_interp_dec )
{
  run_interp_dec_tests<float>();
  run_interp_dec_tests<double>();
}
