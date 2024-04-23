#define BOOST_TEST_MODULE test_non_linear_tools
#include <boost/test/included/unit_test.hpp>

#include <functional>

#include "math/non_linear_tools.hpp"

using namespace std;
using namespace Math::Functions;

template<typename ScalarType>
void run_sgn_test()
{
  // Define the things we want to test
  const ScalarType pos = 10.0;
  const ScalarType zero = 0.0;
  const ScalarType neg = -0.5;

  // Run the actual tests
  BOOST_TEST(sgn(pos) == 1.0);
  BOOST_TEST(sgn(zero) == 1.0);
  BOOST_TEST(sgn(neg) == -1.0);
}

template<typename ScalarType>
void run_no_sat_test(const function<ScalarType(const ScalarType)>& func)
{
  // Define the things we want to test
  const ScalarType pos = 0.3;
  const ScalarType zero = 0.0;
  const ScalarType neg = -0.7;

  // Run the actual tests
  BOOST_TEST(not isnan(func(pos)));
  BOOST_TEST(func(zero) == 0.0);
  BOOST_TEST(not isnan(func(neg)));
}

template<typename ScalarType>
void run_all_no_sat_test()
{
  run_no_sat_test<ScalarType>(&arraya<ScalarType>);
  run_no_sat_test<ScalarType>(&sigmoid2<ScalarType>);
}

template<typename ScalarType>
void run_sat_test(const function<ScalarType(const ScalarType, const ScalarType)>& func)
{
  // Define the things we want to test
  const ScalarType pos = 0.3;
  const ScalarType zero = 0.0;
  const ScalarType neg = -0.7;
  const ScalarType k = 0.5;

  // Run the actual tests
  BOOST_TEST(not isnan(func(pos, k)));
  BOOST_TEST(not isnan(func(zero, k)));
  BOOST_TEST(not isnan(func(neg, k)));
}

template<typename ScalarType>
void run_all_sat_test()
{
  run_sat_test<ScalarType>(&sigmoid<ScalarType>);
  run_sat_test<ScalarType>(&hyperbolic_tangent<ScalarType>);
  run_sat_test<ScalarType>(&arctangent<ScalarType>);
  run_sat_test<ScalarType>(&fuzz_exponential<ScalarType>);
}

template<typename ScalarType>
void run_asymmetrized_test(const function<ScalarType(const ScalarType, const ScalarType)>& func)
{
  // Define the things we want to test
  const ScalarType pos = 0.3;
  const ScalarType zero = 0.0;
  const ScalarType neg = -0.7;
  const ScalarType k = 0.5;
  const ScalarType g = 0.4;

  // Setup the asymmetrized function
  const auto asym_func = asymmetrize(func, g);

  BOOST_TEST(asym_func(pos, k) == func(pos, k));
  BOOST_TEST(asym_func(zero, k) == func(zero, k));
  BOOST_TEST(asym_func(neg, k) != func(neg, k));
}

template<typename ScalarType>
void run_all_asym_test()
{
  run_asymmetrized_test<ScalarType>(&sigmoid<ScalarType>);
  run_asymmetrized_test<ScalarType>(&hyperbolic_tangent<ScalarType>);
  run_asymmetrized_test<ScalarType>(&arctangent<ScalarType>);
  run_asymmetrized_test<ScalarType>(&fuzz_exponential<ScalarType>);
}

BOOST_AUTO_TEST_CASE( test_sgn )
{
  run_sgn_test<float>();
  run_sgn_test<double>();
  run_sgn_test<long double>();
}

BOOST_AUTO_TEST_CASE( test_no_sat_functions )
{
  run_all_no_sat_test<float>();
  run_all_no_sat_test<double>();
  run_all_no_sat_test<long double>();
}

BOOST_AUTO_TEST_CASE( test_sat_functions )
{
  run_all_sat_test<float>();
  run_all_sat_test<double>();
  run_all_sat_test<long double>();
}

BOOST_AUTO_TEST_CASE( test_asym_functions )
{
  run_all_asym_test<float>();
  run_all_asym_test<double>();
  run_all_asym_test<long double>();
}
