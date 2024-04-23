/**
 * This file implements various non-linear functions that are useful in audio modeling
 * and signal processing. Most of the functions are templated, but are really only
 * intended for real, floating point types. Attempting to compile them with other scalar
 * types may not work.
*/

#pragma once

#include <cassert>
#include <cmath>
#include <functional>
#include <numbers>

/**
 * The following non-linear functions are taken from chapter 19.8 of "Designing Audio
 * Effect Plugins in C++" by Will Pirkle. See the following link for more details:
 * https://www.amazon.com/Designing-Audio-Effect-Plugins-C/dp/1138591939
 * Note: In each case, x corresponds to a signal input and k corresponds to a saturation
 * parameter
*/
namespace Math {
namespace Functions {

template<typename ScalarType>
ScalarType sgn(const ScalarType x)
{
  return x >= 0.0 ? 1.0 : -1.0;
}

template<typename ScalarType>
struct ExpConstants
{
  public: // Static Members
    static constexpr ScalarType e_p1 = std::numbers::e + 1.0;
    static constexpr ScalarType e_m1 = std::numbers::e - 1.0;
    static constexpr ScalarType e_p1_div_e_m1 = e_p1 / e_m1;
};

template<typename ScalarType>
ScalarType arraya(const ScalarType x)
{
  return 1.5 * x * (1.0 - (x * x / 3.0));
}

template<typename ScalarType>
ScalarType sigmoid(const ScalarType x, const ScalarType k)
{
  return (2.0 / (1.0 + std::exp(-k * x))) - 1.0;
}

template<typename ScalarType>
ScalarType sigmoid2(const ScalarType x)
{
  return ExpConstants<ScalarType>::e_p1_div_e_m1 * (std::exp(x) - 1.0) / (std::exp(x) + 1.0);
}

template<typename ScalarType>
ScalarType hyperbolic_tangent(const ScalarType x, const ScalarType k)
{
  assert(k != 0.0);
  return std::tanh(k * x) / std::tanh(k);
}

template<typename ScalarType>
ScalarType arctangent(const ScalarType x, const ScalarType k)
{
  assert(k != 0.0);
  return std::atan(k * x) / std::atan(k);
}

template<typename ScalarType>
ScalarType fuzz_exponential(const ScalarType x, const ScalarType k)
{
  assert(k != 0.0);
  return sgn(x) * (1.0 - std::exp(std::abs(k * x))) / (1.0 - std::exp(-k));
}

template<typename ScalarType>
std::function<ScalarType(const ScalarType, const ScalarType)> asymmetrize(
  const std::function<ScalarType(const ScalarType, const ScalarType)>& func,
  const ScalarType g
)
{
  assert(g > 0.0 and g <= 1.0);
  return [g, &func](const ScalarType x, const ScalarType k) {
    if (x >= 0.0) return func(x, k);
    return g * func(x, k / g);
  };
}

} // End namespace Functions
} // End namespace Math
