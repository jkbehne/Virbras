#pragma once

#include <cmath>
#include <vector>

namespace VectorTools {

/**
 * Check if two vectors have all entries within some tolerance of one another
*/
template<typename ScalarType>
bool all_close(
  const std::vector<ScalarType>& a,
  const std::vector<ScalarType>& b,
  const double tolerance
)
{
  if (a.size() != b.size()) return false;
  for (int i = 0; i < a.size(); ++i)
  {
    if (abs(a[i] - b[i]) > tolerance) return false;
  }
  return true;
}

}
