/**
 * The tests within this file all look at implementing a feedforward feedback comb filter
 * with the difference equation:
 * 
 * y[n] = x[n] + x[n - 3] - 0.5 * y[n - 3]
 * 
 * with x[n] = [1, 2, 3, 4]
 * which yields y[n] = [1, 2, 3, 4.5, 1, 1.5, 1.75, -0.5, -0.75, -0.875]
 * when 6 additional filter transients are added (though note again that this is
 * technically the output of an IIR fitler and will go on forever).
 * 
 * In the complex case, we just duplicate all of the quantities in both the real and
 * imaginary parts.
*/
#define BOOST_TEST_MODULE test_feedback_comb_filter
#include <boost/test/included/unit_test.hpp>

#include <complex>

#include "math/vector_tools.hpp"
#include "signal/feedback_comb_filter.hpp"

using namespace std;
using namespace VectorTools;

// Do some typedefs
typedef STLVectorOutputSignal<double> DoubleOutputType;
typedef STLVectorOutputSignal<complex<double>> CDoubleOutputType;

// Define some constants
const double input_coeff = 1.0;
const double delayed_input_coeff = 1.0;
const double delayed_output_coeff = -0.5;
const int delay = 3;
const int num_transients = 6;
const double tolerance = 1e-10;

BOOST_AUTO_TEST_CASE( test_double_fffbcf )
{
  // Create the expected result
  const vector<double> expected {1.0, 2.0, 3.0, 4.5, 1.0, 1.5, 1.75, -0.5, -0.75, -0.875};

  // Create the input and output streams
  const shared_ptr<InputSignalStream<double>> isignal = std::make_shared<STLVectorInputSignal<double>>(
    vector<double>{1.0, 2.0, 3.0, 4.0}
  );
  const shared_ptr<OutputSignalStream<double>> osignal = std::make_shared<DoubleOutputType>(10);

  // Create the comb filter
  auto cf = FeedforwardFeedbackCombFilter<double>(
    input_coeff, delayed_input_coeff, delayed_output_coeff, delay
  );

  // Run the filter and check the result
  cf.process(isignal, osignal, num_transients);
  const auto output = (dynamic_cast<const DoubleOutputType&>(*osignal)).output;
  BOOST_TEST(all_close(output, expected, tolerance));
}

BOOST_AUTO_TEST_CASE( test_cdouble_fffbcf )
{
  // Create the expected result
  const vector<complex<double>> expected {
    {1.0, 1.0},
    {2.0, 2.0},
    {3.0, 3.0},
    {4.5, 4.5},
    {1.0, 1.0},
    {1.5, 1.5},
    {1.75, 1.75},
    {-0.5, -0.5},
    {-0.75, -0.75},
    {-0.875, -0.875}
  };

  // Create the input and output streams
  const shared_ptr<InputSignalStream<complex<double>>> isignal = std::make_shared<STLVectorInputSignal<complex<double>>>(
    vector<complex<double>>{{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}, {4.0, 4.0}}
  );
  const shared_ptr<OutputSignalStream<complex<double>>> osignal = std::make_shared<CDoubleOutputType>(10);

  // Create the comb filter
  auto cf = FeedforwardFeedbackCombFilter<complex<double>>(
    {input_coeff, 0.0},
    {delayed_input_coeff, 0.0},
    {delayed_output_coeff, 0.0},
    delay
  );

  // Run the filter and check the result
  cf.process(isignal, osignal, num_transients);
  const auto output = (dynamic_cast<const CDoubleOutputType&>(*osignal)).output;
  BOOST_TEST(all_close(output, expected, tolerance));
}
