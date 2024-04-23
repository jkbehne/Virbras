/**
 * This file is intended to house various utilities that are useful in the realm of signal
 * processing. However, the intent is that only very small and simple functions will be
 * stored here. For anything more complicated, create a separate file.
*/
#pragma once

#include <cmath>

namespace Signal {
namespace Utils {

namespace detail {

template<typename ScalarType>
struct dBExponent
{
  public: // Members
    static constexpr ScalarType scale = 1.0 / 20.0;
};

} // End namespace detail

template<typename ScalarType>
ScalarType from_dB(const ScalarType input_dB)
{
  return std::pow(10.0, detail::dBExponent<ScalarType>::scale * input_dB);
}

} // End namespace Utils
} // End namespace Signal
