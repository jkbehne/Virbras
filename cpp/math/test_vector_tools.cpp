#define BOOST_TEST_MODULE test_vector_tools
#include <boost/test/included/unit_test.hpp>

#include <complex>

#include "math/vector_tools.hpp"

using namespace VectorTools;
using namespace std;

BOOST_AUTO_TEST_CASE ( test_all_close )
{
  // Define constants
  const double tolerance = 1e-9;

  // Test real-valued vectors
  const vector<double> vec1 {0.0, 1.0, 2.0, 3.0};
  const vector<double> vec2 {0.0, 1.0, 2.0, 3.0};
  const vector<double> vec3 {0.0, 0.0, 0.0, 0.0};
  const vector<double> vec4 {0.0, 0.0, 0.0};

  const auto alc1 = all_close(vec1, vec2, tolerance);
  BOOST_TEST(alc1);
  const auto alc2 = all_close(vec2, vec3, tolerance);
  BOOST_TEST(not alc2);

  // Test that vectors of different sizes always produce false
  const auto b1 = all_close(vec1, vec4, tolerance);
  const auto b2 = all_close(vec2, vec4, tolerance);
  const auto b3 = all_close(vec3, vec4, tolerance);

  BOOST_TEST(not b1);
  BOOST_TEST(not b2);
  BOOST_TEST(not b3);

  // Test complex-valued vectors
  const vector<complex<double>> cvec1 {
    complex<double>{0.0, 0.0},
    complex<double>{1.0, 1.0},
    complex<double>{2.0, 2.0},
    complex<double>{3.0, 3.0}
  };
  const vector<complex<double>> cvec2 {
    complex<double>{0.0, 0.0},
    complex<double>{1.0, 1.0},
    complex<double>{2.0, 2.0},
    complex<double>{3.0, 3.0}
  };
  const vector<complex<double>> cvec3 {
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0}
  };
  const vector<complex<double>> cvec4 {
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0}
  };

  const auto calc1 = all_close(cvec1, cvec2, tolerance);
  BOOST_TEST(calc1);
  const auto calc2 = all_close(cvec2, cvec3, tolerance);
  BOOST_TEST(not calc2);

  // Test that vectors of different sizes always produce false
  const auto cb1 = all_close(cvec1, cvec4, tolerance);
  const auto cb2 = all_close(cvec2, cvec4, tolerance);
  const auto cb3 = all_close(cvec3, cvec4, tolerance);

  BOOST_TEST(not cb1);
  BOOST_TEST(not cb2);
  BOOST_TEST(not cb3);
}