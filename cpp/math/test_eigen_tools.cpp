#define BOOST_TEST_MODULE test_eigen_tools
#include <boost/test/included/unit_test.hpp>

#include <complex>
#include <iostream>

#include "math/eigen_tools.hpp"

using namespace EigenTools;
using namespace std;

BOOST_AUTO_TEST_CASE ( test_vector )
{
  // Define constants
  const double tolerance = 1e-9;

  // Test real-valued vectors with compile-time sizes
  const Vector4d vec1 {0.0, 1.0, 2.0, 3.0};
  const Vector4d vec2 {0.0, 1.0, 2.0, 3.0};
  const Vector4d vec3 {0.0, 0.0, 0.0, 0.0};

  const auto alc1 = all_close(vec1, vec2, tolerance);
  BOOST_TEST(alc1);
  const auto alc2 = all_close(vec2, vec3, tolerance);
  BOOST_TEST(not alc2);

  // Test complex-valued vectors with compile-time sizes
  const Vector4cd cvec1 {
    complex<double>{0.0, 0.0},
    complex<double>{1.0, 1.0},
    complex<double>{2.0, 2.0},
    complex<double>{3.0, 3.0}
  };
  const Vector4cd cvec2 {
    complex<double>{0.0, 0.0},
    complex<double>{1.0, 1.0},
    complex<double>{2.0, 2.0},
    complex<double>{3.0, 3.0}
  };
  const Vector4cd cvec3 {
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0}
  };

  const auto calc1 = all_close(cvec1, cvec2, tolerance);
  BOOST_TEST(calc1);
  const auto calc2 = all_close(cvec2, cvec3, tolerance);
  BOOST_TEST(not calc2);

  // Test real-valued vectors with dynamic sizes
  VectorXd xvec1 {{1.0, 2.0, 3.0, 4.0}};
  VectorXd xvec2 {{1.0, 2.0, 3.0, 4.0}};
  VectorXd xvec3 {{0.0, 0.0, 0.0, 0.0}};

  const auto xalc1 = all_close(xvec1, xvec2, tolerance);
  BOOST_TEST(xalc1);
  const auto xalc2 = all_close(xvec2, xvec3, tolerance);
  BOOST_TEST(not xalc2);

  // Test complex-valued vectors with dynamic sizes
  const VectorXcd xcvec1 {{
    complex<double>{0.0, 0.0},
    complex<double>{1.0, 1.0},
    complex<double>{2.0, 2.0},
    complex<double>{3.0, 3.0}
  }};
  const VectorXcd xcvec2 {{
    complex<double>{0.0, 0.0},
    complex<double>{1.0, 1.0},
    complex<double>{2.0, 2.0},
    complex<double>{3.0, 3.0}
  }};
  const VectorXcd xcvec3 {{
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0},
    complex<double>{0.0, 0.0}
  }};

  const auto cxalc1 = all_close(xcvec1, xcvec2, tolerance);
  BOOST_TEST(cxalc1);
  const auto cxalc2 = all_close(xcvec2, xcvec3, tolerance);
  BOOST_TEST(not cxalc2);
}

// TODO: Test the matrix cases if there are future applications for it
