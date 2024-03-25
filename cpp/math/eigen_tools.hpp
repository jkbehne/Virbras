#pragma once

#include <cmath>
#include <complex>
#include <type_traits>

#include <eigen3/Eigen/Dense>

namespace EigenTools {

using namespace Eigen;
using namespace std;

/**
 * Check if a matrix of double floating points is all close
 * TODO: This function will currently only compile for double
 * scalar types (real and complex) and this could be extended
 * if desired.
*/
template<typename ScalarType, int Rows, int Cols>
constexpr bool all_close(
  const Matrix<ScalarType, Rows, Cols>& a,
  const Matrix<ScalarType, Rows, Cols>& b,
  const double tolerance)
{
  static_assert(
    std::is_same_v<ScalarType, double> or
    std::is_same_v<ScalarType, complex<double>>
  );
  if constexpr(Rows == Dynamic)
  {
    assert(a.rows() == b.rows());
  }
  if constexpr(Cols == Dynamic)
  {
    assert(a.cols() == b.cols());
  }
  for (int i = 0; i < a.rows(); ++i)
  {
    for (int j = 0; j < a.cols(); ++j)
    {
      if (abs(a(i, j) - b(i, j)) >= tolerance) return false;
    }
  }
  return true;
}
}
