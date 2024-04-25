#pragma once

#include <cmath>
#include <iostream>
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

} // End namespace VectorTools

namespace Signal {

template<typename ScalarType>
std::ostream& operator<<(std::ostream& stream, const std::vector<ScalarType>& input)
{
  stream << "[";
  for (unsigned int i = 0; i < input.size(); ++i)
  {
    stream << input[i];
    if (i == input.size() - 1) stream << "]";
    else stream << ", ";
  }
  return stream;
}

} // End namespace Signal
