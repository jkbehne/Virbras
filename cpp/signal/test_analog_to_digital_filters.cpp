#define BOOST_TEST_MODULE test_analog_to_digital_filters
#include <boost/test/included/unit_test.hpp>

#include "math/vector_tools.hpp"
#include "signal/analog_to_digital_filters.hpp"

using namespace std;
using namespace Signal;
using namespace VectorTools;

template<typename ScalarType>
void run_first_order_filter_tests()
{
  const vector<ScalarType> input {0.0, 1.0, 2.0, 3.0, 4.0};

  // Test that we replicate the input if dry=1 and wet=0
  FirstOrderFilter<ScalarType> filter_dry(
    1.0 /* dry */,
    0.0 /* wet */,
    5.0 /* a0 */,
    5.0 /* a1 */,
    5.0 /* b1 */,
    FirstOrderFilterType::Lowpass /* filter type - not necessarily lowpass */
  );
  const auto in_replica = run_filter(input, filter_dry, 0 /* num_transients */);
  BOOST_CHECK_EQUAL_COLLECTIONS(input.begin(), input.end(), in_replica.begin(), in_replica.end());

  // Test that we replicate the input if dry=0, wet=1, a0=1, a1=0, and b1=0
  FirstOrderFilter<ScalarType> filter_rep(
    0.0 /* dry */,
    1.0 /* wet */,
    1.0 /* a0 */,
    0.0 /* a1 */,
    0.0 /* b1 */,
    FirstOrderFilterType::Lowpass /* filter type - not necessarily lowpass */
  );
  const auto in_replica2 = run_filter(input, filter_rep, 0 /* num_transients */);
  BOOST_CHECK_EQUAL_COLLECTIONS(input.begin(), input.end(), in_replica2.begin(), in_replica2.end());

  // Test that we just delay the signal if dry=0, wet=1, a0=0, a1=1, and b1=0
  FirstOrderFilter<ScalarType> filter_delay(
    0.0 /* dry */,
    1.0 /* wet */,
    0.0 /* a0 */,
    1.0 /* a1 */,
    0.0 /* b1 */,
    FirstOrderFilterType::Lowpass /* filter type - not necessarily lowpass */
  );
  const auto delayed = run_filter(input, filter_delay, 1 /* num_transients */);
  const vector<ScalarType> expected_delayed {0.0, 0.0, 1.0, 2.0, 3.0, 4.0};
  BOOST_CHECK_EQUAL_COLLECTIONS(delayed.begin(), delayed.end(), expected_delayed.begin(), expected_delayed.end());

  // Test that we get all zeros if dry=0, wet=1, a0=0, a1=0, and b1=0
  FirstOrderFilter<ScalarType> filter_zero(
    0.0 /* dry */,
    1.0 /* wet */,
    0.0 /* a0 */,
    0.0 /* a1 */,
    0.0 /* b1 */,
    FirstOrderFilterType::Lowpass /* filter type - not necessarily lowpass */
  );
  const auto zeros = run_filter(input, filter_zero, 2 /* num_transients */);
  const vector<ScalarType> expected_zeros {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  BOOST_CHECK_EQUAL_COLLECTIONS(zeros.begin(), zeros.end(), expected_zeros.begin(), expected_zeros.end());

  // Test the output if dry=0, wet=1, a0=1, a1=1, and b1=0.5
  FirstOrderFilter<ScalarType> filter(
    0.0 /* dry */,
    1.0 /* wet */,
    1.0 /* a0 */,
    1.0 /* a1 */,
    0.5 /* b1 */,
    FirstOrderFilterType::Lowpass /* filter type - not necessarily lowpass */
  );
  const auto out = run_filter(input, filter, 2 /* num_transients */);
  const vector<ScalarType> expected {0.0, 1.0, 2.5, 3.75, 5.125, 1.4375, -0.71875};
  BOOST_TEST(all_close(out, expected, 1e-7));
}

template<typename ScalarType>
void run_lowpass_tests()
{
  // Check that we get the expected coefficients at cutoff of 0, 1, and 0.5
  const auto filter0 = make_lowpass_first_order<ScalarType>(
    0.0 /* dry */,
    1.0 /* wet */,
    0.0, /* cutoff_freq */
    1.0 /* sample_freq */
  );
  BOOST_TEST(filter0.dry == 0.0);
  BOOST_TEST(filter0.wet == 1.0);
  BOOST_TEST(filter0.a0 == 0.0);
  BOOST_TEST(filter0.a1 == 0.0);
  BOOST_TEST(filter0.b1 == -1.0);
  BOOST_TEST(filter0.filter_type == FirstOrderFilterType::Lowpass);

  const auto filter1 = make_lowpass_first_order<ScalarType>(
    0.0 /* dry */,
    1.0 /* wet */,
    1.0, /* cutoff_freq */
    1.0 /* sample_freq */
  );
  BOOST_TEST(filter1.dry == 0.0);
  BOOST_TEST(filter1.wet == 1.0);
  BOOST_TEST(filter1.a0 == 0.0);
  BOOST_TEST(filter1.a1 == 0.0);
  BOOST_TEST(filter1.b1 == -1.0);
  BOOST_TEST(filter1.filter_type == FirstOrderFilterType::Lowpass);

  const auto filterh = make_lowpass_first_order<ScalarType>(
    0.0 /* dry */,
    1.0 /* wet */,
    0.5, /* cutoff_freq */
    1.0 /* sample_freq */
  );
  BOOST_TEST(filterh.dry == 0.0);
  BOOST_TEST(filterh.wet == 1.0);
  BOOST_TEST(filterh.a0 == 1.0);
  BOOST_TEST(filterh.a1 == 1.0);
  BOOST_TEST(filterh.b1 == 1.0);
  BOOST_TEST(filterh.filter_type == FirstOrderFilterType::Lowpass);
}

template<typename ScalarType>
void run_highpass_tests()
{
  // Check that we get the expected coefficients at cutoff of 0, 1, and 0.5
  const auto filter0 = make_highpass_first_order<ScalarType>(
    0.0 /* dry */,
    1.0 /* wet */,
    0.0, /* cutoff_freq */
    1.0 /* sample_freq */
  );
  BOOST_TEST(filter0.dry == 0.0);
  BOOST_TEST(filter0.wet == 1.0);
  BOOST_TEST(filter0.a0 == 1.0);
  BOOST_TEST(filter0.a1 == -1.0);
  BOOST_TEST(filter0.b1 == -1.0);
  BOOST_TEST(filter0.filter_type == FirstOrderFilterType::Highpass);

  const auto filter1 = make_highpass_first_order<ScalarType>(
    0.0 /* dry */,
    1.0 /* wet */,
    1.0, /* cutoff_freq */
    1.0 /* sample_freq */
  );
  BOOST_TEST(filter1.dry == 0.0);
  BOOST_TEST(filter1.wet == 1.0);
  BOOST_TEST(filter1.a0 == 1.0);
  BOOST_TEST(filter1.a1 == -1.0);
  BOOST_TEST(filter1.b1 == -1.0);
  BOOST_TEST(filter1.filter_type == FirstOrderFilterType::Highpass);

  const auto filterh = make_highpass_first_order<ScalarType>(
    0.0 /* dry */,
    1.0 /* wet */,
    0.5, /* cutoff_freq */
    1.0 /* sample_freq */
  );
  BOOST_TEST(filterh.dry == 0.0);
  BOOST_TEST(filterh.wet == 1.0);
  BOOST_TEST(filterh.a0 == 0.0);
  BOOST_TEST(filterh.a1 == 0.0);
  BOOST_TEST(filterh.b1 == 1.0);
  BOOST_TEST(filterh.filter_type == FirstOrderFilterType::Highpass);
}

template<typename ScalarType>
void run_low_shelf_tests()
{
  // Check that we get the expected coefficients at cutoff of 0, 1, and 0.5
  const auto filter0 = make_low_shelf_first_order<ScalarType>(
    0.0 /* cutoff_freq */,
    1.0 /* sample_freq */,
    20.0 /* gain_dB */
  );
  BOOST_TEST(filter0.dry == 1.0);
  BOOST_TEST(filter0.wet == 9.0);
  BOOST_TEST(filter0.a0 == 0.0);
  BOOST_TEST(filter0.a1 == 0.0);
  BOOST_TEST(filter0.b1 == -1.0);
  BOOST_TEST(filter0.filter_type == FirstOrderFilterType::LowShelving);

  const auto filter1 = make_low_shelf_first_order<ScalarType>(
    1.0 /* cutoff_freq */,
    1.0 /* sample_freq */,
    20.0 /* gain_dB */
  );
  BOOST_TEST(filter1.dry == 1.0);
  BOOST_TEST(filter1.wet == 9.0);
  BOOST_TEST(filter1.a0 == 0.0);
  BOOST_TEST(filter1.a1 == 0.0);
  BOOST_TEST(filter1.b1 == -1.0);
  BOOST_TEST(filter1.filter_type == FirstOrderFilterType::LowShelving);

  const auto filterh = make_low_shelf_first_order<ScalarType>(
    0.25 /* cutoff_freq */,
    1.0 /* sample_freq */,
    20.0 /* gain_dB */
  );
  BOOST_TEST(filterh.dry == 1.0);
  BOOST_TEST(filterh.wet == 9.0);

  const ScalarType delta = 4.0 / 11.0;
  const ScalarType gammas = (1.0 - delta) / (1.0 + delta);

  BOOST_TEST(filterh.a0 == 0.5 * (1.0 - ((1.0 - (4.0 / 11.0)) / (1.0 + (4.0 / 11.0)))));
  BOOST_TEST(filterh.a1 == 0.5 * (1.0 - ((1.0 - (4.0 / 11.0)) / (1.0 + (4.0 / 11.0)))));
  BOOST_TEST(filterh.b1 == -(1.0 - (4.0 / 11.0)) / (1.0 + (4.0 / 11.0)));
  BOOST_TEST(filterh.filter_type == FirstOrderFilterType::LowShelving);
}

template<typename ScalarType>
void run_high_shelf_tests()
{
  // Check that we get the expected coefficients at cutoff of 0, 1, and 0.5
  const auto filter0 = make_high_shelf_first_order<ScalarType>(
    0.0 /* cutoff_freq */,
    1.0 /* sample_freq */,
    20.0 /* gain_dB */
  );
  BOOST_TEST(filter0.dry == 1.0);
  BOOST_TEST(filter0.wet == 9.0);
  BOOST_TEST(filter0.a0 == 1.0);
  BOOST_TEST(filter0.a1 == -1.0);
  BOOST_TEST(filter0.b1 == -1.0);
  BOOST_TEST(filter0.filter_type == FirstOrderFilterType::HighShelving);

  const auto filter1 = make_high_shelf_first_order<ScalarType>(
    1.0 /* cutoff_freq */,
    1.0 /* sample_freq */,
    20.0 /* gain_dB */
  );
  BOOST_TEST(filter1.dry == 1.0);
  BOOST_TEST(filter1.wet == 9.0);
  BOOST_TEST(filter1.a0 == 1.0);
  BOOST_TEST(filter1.a1 == -1.0);
  BOOST_TEST(filter1.b1 == -1.0);
  BOOST_TEST(filter1.filter_type == FirstOrderFilterType::HighShelving);

  const auto filterh = make_high_shelf_first_order<ScalarType>(
    0.25 /* cutoff_freq */,
    1.0 /* sample_freq */,
    20.0 /* gain_dB */
  );
  BOOST_TEST(filterh.dry == 1.0);
  BOOST_TEST(filterh.wet == 9.0);

  const ScalarType delta = 4.0 / 11.0;
  const ScalarType gammas = (1.0 - delta) / (1.0 + delta);

  BOOST_TEST(filterh.a0 == 0.5 * (1.0 + ((1.0 - (11.0 / 4.0)) / (1.0 + (11.0 / 4.0)))));
  BOOST_TEST(filterh.a1 == -0.5 * (1.0 + ((1.0 - (11.0 / 4.0)) / (1.0 + (11.0 / 4.0)))));
  BOOST_TEST(filterh.b1 == -(1.0 - (11.0 / 4.0)) / (1.0 + (11.0 / 4.0)));
  BOOST_TEST(filterh.filter_type == FirstOrderFilterType::HighShelving);
}

BOOST_AUTO_TEST_CASE( test_first_order_filter )
{
  run_first_order_filter_tests<float>();
  run_first_order_filter_tests<double>();
}

BOOST_AUTO_TEST_CASE( test_lowpass_filter, * boost::unit_test::tolerance(1e-6) )
{
  run_lowpass_tests<float>();
  run_lowpass_tests<double>();
}

BOOST_AUTO_TEST_CASE( test_highpass_filter, * boost::unit_test::tolerance(1e-6) )
{
  run_highpass_tests<float>();
  run_highpass_tests<double>();
}

BOOST_AUTO_TEST_CASE( test_low_shelf_filter, * boost::unit_test::tolerance(1e-6) )
{
  run_low_shelf_tests<float>();
  run_low_shelf_tests<double>();
}

BOOST_AUTO_TEST_CASE( test_high_shelf_filter, * boost::unit_test::tolerance(1e-6) )
{
  run_high_shelf_tests<float>();
  run_high_shelf_tests<double>();
}
