/**
 * This file contains some simple digital filters that are designed via the bilinear transform
 * from various analog filter architectures. Warping is included and the definitions are taken 
 * from Chapter 11.3 of Designing Audio Effects Plugins in C++ by Will Pirkle. See:
 * https://www.willpirkle.com/ for more details on the book.
*/
#pragma once

#include <cmath>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

namespace Signal {

template<typename ScalarType, typename FilterType>
std::vector<ScalarType> run_filter(
  const std::vector<ScalarType>& input,
  FilterType& filter,
  const unsigned int num_transients
)
{
  // Setup the output
  std::vector<ScalarType> out(input.size() + num_transients);

  // Run the filter
  for (unsigned int i = 0; i < input.size(); ++i)
  {
    out[i] = filter.next(input[i]);
  }

  // Add the transients
  for (unsigned int i = 0; i < num_transients; ++i)
  {
    out[input.size() + i] = filter.next({});
  }

  return out;
}

enum class FirstOrderFilterType { Lowpass, Highpass, LowShelving, HighShelving };

// TODO: There's probably cleaner / more reusable ways to do this
template<typename EnumType, typename = std::enable_if<std::is_enum<EnumType>::value>::type>
std::ostream& operator<<(std::ostream& stream, const EnumType& en)
{
  // static_assert(std::is_enum<EnumType>::value);
  return stream << static_cast<typename std::underlying_type<EnumType>::type>(en);
}

/**
 * Basic class that implements the finite difference equation:
 * y[n] = dry * x[n] + wet * (a0 * x[n] + a1 * x[n - 1] - b1 * y[n - 1])
 * and includes an enum member to denote the type of filter
*/
template<typename ScalarType>
class FirstOrderFilter
{
  public: // Members
    ScalarType dry;
    ScalarType wet;

    ScalarType a0;
    ScalarType a1;
    ScalarType b1;

    FirstOrderFilterType filter_type;

  private: // Members
    ScalarType x_prev;
    ScalarType y_prev;

  public: // Methods
    FirstOrderFilter(
      const ScalarType dry_,
      const ScalarType wet_,
      const ScalarType a0_,
      const ScalarType a1_,
      const ScalarType b1_,
      const FirstOrderFilterType filter_type_
    ) : dry(dry_),
        wet(wet_),
        a0(a0_),
        a1(a1_),
        b1(b1_),
        filter_type(filter_type_),
        x_prev(),
        y_prev() {}

    // TODO: We could consider implementing different forms of this equation
    ScalarType next(const ScalarType x)
    {
      // Compute the output
      const ScalarType out = dry * x + wet * (a0 * x + a1 * x_prev - b1 * y_prev);

      // Update the state
      x_prev = x;
      y_prev = out;
      return out;
    }
};


namespace detail {

/**
 * Computes the intermediate gamma value for 1st order lowpass / highpass filters
*/
template<typename ScalarType>
ScalarType compute_gamma(
  const ScalarType cutoff_freq,
  const ScalarType sample_freq
)
{
  const ScalarType theta_c = 2.0 * M_PI * cutoff_freq / sample_freq;
  return std::cos(theta_c) / (1.0 + std::sin(theta_c));
}

} // End namespace detail

template<typename ScalarType>
FirstOrderFilter<ScalarType> make_lowpass_first_order(
  const ScalarType dry,
  const ScalarType wet,
  const ScalarType cutoff_freq,
  const ScalarType sample_freq
)
{
  // Compute the parameters for the lowpass filter
  const auto gamma = detail::compute_gamma(cutoff_freq, sample_freq);
  const ScalarType a0 = 0.5 * (1.0 - gamma);
  const ScalarType a1 = 0.5 * (1.0 - gamma);
  const ScalarType b1 = -gamma;

  // Return the resulting filter
  return FirstOrderFilter(dry, wet, a0, a1, b1, FirstOrderFilterType::Lowpass);
}

template<typename ScalarType>
FirstOrderFilter<ScalarType> make_highpass_first_order(
  const ScalarType dry,
  const ScalarType wet,
  const ScalarType cutoff_freq,
  const ScalarType sample_freq
)
{
  // Compute the parameters for the highpass filter
  const auto gamma = detail::compute_gamma(cutoff_freq, sample_freq);
  const ScalarType a0 = 0.5 * (1.0 + gamma);
  const ScalarType a1 = -0.5 * (1.0 + gamma);
  const ScalarType b1 = -gamma;

  // Return the resulting filter
  return FirstOrderFilter(dry, wet, a0, a1, b1, FirstOrderFilterType::Highpass);
}

namespace detail {

/**
 * Computes the intermediate gamma / mu values for low / high shelf filters
*/
template<typename ScalarType, bool IsLowShelving>
std::pair<ScalarType, ScalarType> compute_gamma_mu(
  const ScalarType cutoff_freq,
  const ScalarType sample_freq,
  const ScalarType gain_dB
)
{
  const ScalarType theta_c = 2.0 * M_PI * cutoff_freq / sample_freq;
  const ScalarType mu = std::pow(10.0, gain_dB / 20.0);
  ScalarType beta;
  if constexpr (IsLowShelving) beta = 4.0 / (1.0 + mu);
  else beta = 0.25 * (1.0 + mu);
  const ScalarType delta = beta * std::tan(0.5 * theta_c);
  const ScalarType gamma = (1.0 - delta) / (1.0 + delta);
  return {gamma, mu};
}

} // End namespace detail

template<typename ScalarType>
FirstOrderFilter<ScalarType> make_low_shelf_first_order(
  const ScalarType cutoff_freq,
  const ScalarType sample_freq,
  const ScalarType gain_dB
)
{
  // Compute the parameters of the low shelf filter
  const auto gm = detail::compute_gamma_mu<ScalarType, true>(cutoff_freq, sample_freq, gain_dB);
  const auto gamma = gm.first;
  const auto mu = gm.second;
  const ScalarType a0 = 0.5 * (1.0 - gamma);
  const ScalarType a1 = 0.5 * (1.0 - gamma);
  const ScalarType b1 = -gamma;

  // Return the resulting filter
  return FirstOrderFilter(
    static_cast<ScalarType>(1.0) /* dry */,
    static_cast<ScalarType>(mu - 1.0) /* wet */,
    a0, a1, b1, FirstOrderFilterType::LowShelving
  );
}

template<typename ScalarType>
FirstOrderFilter<ScalarType> make_high_shelf_first_order(
  const ScalarType cutoff_freq,
  const ScalarType sample_freq,
  const ScalarType gain_dB
)
{
  // Compute the parameters of the low shelf filter
  const auto gm = detail::compute_gamma_mu<ScalarType, false>(cutoff_freq, sample_freq, gain_dB);
  const auto gamma = gm.first;
  const auto mu = gm.second;
  const ScalarType a0 = 0.5 * (1.0 + gamma);
  const ScalarType a1 = -0.5 * (1.0 + gamma);
  const ScalarType b1 = -gamma;

  // Return the resulting filter
  return FirstOrderFilter(
    static_cast<ScalarType>(1.0) /* dry */,
    static_cast<ScalarType>(mu - 1.0) /* wet */,
    a0, a1, b1, FirstOrderFilterType::HighShelving
  );
}
} // End namespace Signal
