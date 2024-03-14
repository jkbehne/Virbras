#include "/Users/josh/Virbras/cpp/math/oscillations.hpp"

#include <cmath>

#include <assert.h>

using namespace std;
using namespace Eigen;

SimpleOscillator::SimpleOscillator(
  const double A_,
  const double omega_,
  const double phi_
) : A(A_), omega(omega_), phi(phi_) {}

VectorXcd SimpleOscillator::operator()(const VectorXd& times) const
{
  const auto num_times = times.size();
  const auto arg = cj1 * (omega * times + phi * VectorXd::Ones(num_times)).cast<complex<double>>();
  return A * arg.array().exp();
}

SimpleOscillator SimpleOscillator::operator*(const double scale) const
{
  const auto new_A = scale * A;
  return SimpleOscillator(new_A, omega, phi);
}

SimpleOscillator SimpleOscillator::operator*(const SimpleOscillator& other) const
{
  return SimpleOscillator(A * other.A, omega + other.omega, phi + other.phi);
}

SimpleOscillator SimpleOscillator::operator+(const SimpleOscillator& other) const
{ 
  assert(omega == other.omega);
  const auto cos_sum = A * cos(phi) + other.A * cos(other.phi);
  const auto cos_sum2 = pow(cos_sum, 2.0);
  const auto sin_sum = A * sin(phi) + other.A * sin(other.phi);
  const auto sin_sum2 = pow(sin_sum, 2.0);
  return SimpleOscillator(sqrt(cos_sum2 + sin_sum2), omega, atan2(sin_sum, cos_sum));
}

SimpleOscillator SimpleOscillator::operator-(const SimpleOscillator& other) const
{
  return *this + (other * -1.0);
}

SimpleOscillator SimpleOscillator::operator/(const double scale) const
{
  assert(scale != 0.0);
  return (1.0 / scale) * (*this);
}

SimpleOscillator SimpleOscillator::operator/(const SimpleOscillator& other) const
{
  assert(other.A != 0.0);
  return *this * SimpleOscillator(1.0 / other.A, -1.0 * other.omega, -1.0 * other.phi);
}

SimpleOscillator operator*(const double scale, const SimpleOscillator& so)
{
  return so * scale;
}

SimpleOscillator operator/(const double scale, const SimpleOscillator& so)
{
  return SimpleOscillator(scale, 0.0, 0.0) / so;
}

SimpleOscillator operator-(const SimpleOscillator& so)
{
  return -1.0 * so;
}
