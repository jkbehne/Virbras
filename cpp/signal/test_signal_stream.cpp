#define BOOST_TEST_MODULE test_signal_stream
#include <boost/test/included/unit_test.hpp>

#include <complex>
#include <vector>

#include "signal/signal_stream.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( test_stl_streams )
{
  // Setup the expected results
  const vector<int> int_expected {1, 2, 3, 4};
  const vector<double> db_expected {1.0, 2.0, 3.0, 4.0};
  const vector<complex<double>> dbc_expected {{1.0, 0.0}, {2.0, 0.0}};

  // Test the integer case
  auto int_signal = STLVectorInputSignal(vector<int>{1, 2, 3, 4});
  auto int_output = vector<int>();
  for (int i = 0; i < int_signal.input.size(); ++i)
  {
    const auto result = int_signal.read_next();
    BOOST_REQUIRE(result);
    int_output.push_back(*result);
  }
  BOOST_TEST(int_expected == int_output);
  BOOST_TEST(not int_signal.read_next());

  // Test the double case
  auto db_signal = STLVectorInputSignal(vector<double>{1.0, 2.0, 3.0, 4.0});
  auto db_output = vector<double>();
  for (int i = 0; i < db_signal.input.size(); ++i)
  { 
    const auto result = db_signal.read_next();
    BOOST_REQUIRE(result);
    db_output.push_back(*result);
  }
  BOOST_TEST(db_expected == db_output);
  BOOST_TEST(not db_signal.read_next());

  // Test the complex<double> case
  auto dbc_signal = STLVectorInputSignal(vector<complex<double>>{{1.0, 0.0}, {2.0, 0.0}});
  auto dbc_output = vector<complex<double>>();
  for (int i = 0; i < dbc_signal.input.size(); ++i)
  { 
    const auto result = dbc_signal.read_next();
    BOOST_REQUIRE(result);
    dbc_output.push_back(*result);
  }
  BOOST_TEST(dbc_expected == dbc_output);
  BOOST_TEST(not dbc_signal.read_next());

  // Test that writing integers works
  auto int_osignal_d = STLVectorOutputSignal<int>();
  auto int_osignal = STLVectorOutputSignal<int>(4);

  for (int i = 0; i < int_expected.size(); ++i)
  {
    int_osignal_d.write_next(int_expected[i]);
    int_osignal.write_next(int_expected[i]);
  }
  BOOST_TEST(int_osignal_d.output == int_expected);
  BOOST_TEST(int_osignal.output == int_expected);

  // Test that writing doubles works
  auto db_osignal_d = STLVectorOutputSignal<double>();
  auto db_osignal = STLVectorOutputSignal<double>(4);

  for (int i = 0; i < db_expected.size(); ++i)
  {
    db_osignal_d.write_next(db_expected[i]);
    db_osignal.write_next(db_expected[i]);
  }
  BOOST_TEST(db_osignal_d.output == db_expected);
  BOOST_TEST(db_osignal.output == db_expected);

  // Test that writing complex<double>s works
  auto dbc_osignal_d = STLVectorOutputSignal<complex<double>>();
  auto dbc_osignal = STLVectorOutputSignal<complex<double>>(2);

  for (int i = 0;i < dbc_expected.size(); ++i)
  {
    dbc_osignal_d.write_next(dbc_expected[i]);
    dbc_osignal.write_next(dbc_expected[i]);
  }
  BOOST_TEST(dbc_osignal_d.output == dbc_expected);
  BOOST_TEST(dbc_osignal.output == dbc_expected);
}
