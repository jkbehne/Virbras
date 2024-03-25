/**
 * The tests within this file all look at implementing an FIR
 * filter using a tapped delay line. For each test we take the
 * FIR filter to be h = [4, 5, 7] and we take the input signal
 * to be x = [1, 2, 3, 4]. This works for every type we'd like
 * to test, and we can easily note the correct output is:
 * y = h (conv) x = [4, 13, 29, 45, 41, 28].
*/
#define BOOST_TEST_MODULE test_tapped_delay_line
#include <boost/test/included/unit_test.hpp>

#include <complex>

#include "signal/tapped_delay_line.hpp"

using namespace std;

// Do some typedefs
typedef STLVectorOutputSignal<int> IntOutputType;
typedef STLVectorOutputSignal<double> DoubleOutputType;
typedef STLVectorOutputSignal<complex<double>> CDoubleOutputType;

BOOST_AUTO_TEST_CASE( test_int_tdl )
{
  // Create the expected result
  const vector<int> expected {4, 13, 29, 45, 41, 28};

  // Create the input and output streams
  const shared_ptr<InputSignalStream<int>> isignal = make_shared<STLVectorInputSignal<int>>(
    vector<int>{1, 2, 3, 4}
  );
  const shared_ptr<OutputSignalStream<int>> osignal = make_shared<IntOutputType>(6);

  // Create the TDL
  auto tdl = TappedDelayLine<int>({1, 2}, {4, 5, 7});

  // Run the TDL and check the result
  tdl.process(isignal, osignal);
  const auto output = (dynamic_cast<const IntOutputType&>(*osignal)).output;
  BOOST_TEST(output == expected);
}

BOOST_AUTO_TEST_CASE( test_double_tdl )
{
  // Create the expected result
  const vector<double> expected {4.0, 13.0, 29.0, 45.0, 41.0, 28.0};

  // Create the input and output streams
  const shared_ptr<InputSignalStream<double>> isignal = std::make_shared<STLVectorInputSignal<double>>(
    vector<double>{1.0, 2.0, 3.0, 4.0}
  );
  const shared_ptr<OutputSignalStream<double>> osignal = std::make_shared<DoubleOutputType>(6);

  // Create the TDL
  auto tdl = TappedDelayLine<double>({1, 2}, {4.0, 5.0, 7.0});

  // Run the TDL and check the result
  tdl.process(isignal, osignal);
  const auto output = (dynamic_cast<const DoubleOutputType&>(*osignal)).output;
  BOOST_TEST(output == expected);
}

BOOST_AUTO_TEST_CASE( test_cdouble_tdl )
{
  // Create the expected result
  const vector<complex<double>> expected {
    {4.0, 0.0},
    {13.0, 0.0},
    {29.0, 0.0},
    {45.0, 0.0},
    {41.0, 0.0},
    {28.0, 0.0}
  };

  // Create the input and output streams
  const shared_ptr<InputSignalStream<complex<double>>> isignal = std::make_shared<STLVectorInputSignal<complex<double>>>(
    vector<complex<double>>{{1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}}
  );
  const shared_ptr<OutputSignalStream<complex<double>>> osignal = std::make_shared<CDoubleOutputType>(6);

  // Create the TDL
  auto tdl = TappedDelayLine<complex<double>>(
    {1, 2}, {{4.0, 0.0}, {5.0, 0.0}, {7.0, 0.0}}
  );

  // Run the TDL and check the result
  tdl.process(isignal, osignal);
  const auto output = (dynamic_cast<const CDoubleOutputType&>(*osignal)).output;
  BOOST_TEST(output == expected);
}
