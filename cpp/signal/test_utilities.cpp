#define BOOST_TEST_MODULE test_utilities
#include <boost/test/included/unit_test.hpp>

#include "signal/utilities.hpp"

using namespace Signal::Utils;

template<typename ScalarType>
void run_basic_tests()
{
  BOOST_TEST(from_dB<ScalarType>(0.0) == 1.0);
  BOOST_TEST(from_dB<ScalarType>(-20.0) == 0.1);
  BOOST_TEST(from_dB<ScalarType>(20.0) == 10.0);
}

BOOST_AUTO_TEST_CASE( test_from_dB, * boost::unit_test::tolerance(1e-7) )
{
  run_basic_tests<float>();
  run_basic_tests<double>();
}
