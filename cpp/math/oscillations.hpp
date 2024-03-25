/**
 * This file is intended to house basic mathematical functions relating to oscillations.
 * It should probably only ever contain functionality related to relatively simple forms
 * of oscillations, as that subject is quite complex in general.
*/
#pragma once

#include <complex>
#include <iostream>

#include <eigen3/Eigen/Dense>

inline constexpr std::complex<double> cj1(0.0, 1.0);

/**
 * Implements a simple, complex oscillator, which can mathematically be represented as:
 *
 * x(t) = A * exp(j * (omega * t + phi))
 * 
 * This representation lends itself to a fair number of operators, which are
 * implemented for convenience by this class
*/
class SimpleOscillator
{
  public: // Members
    const double A;
    const double omega;
    const double phi;

  public: // Methods
    /**
     * Construct a SimpleOscillator from basic parameters
    */
    SimpleOscillator(const double, const double, const double);

    /**
     * Evaluate the oscillator at a set of times
    */
    Eigen::VectorXcd operator()(const Eigen::VectorXd&) const;

    /**
     * Multiply the oscillator by a double
     * 
     * Note: One can multiply by a complex constant by multiplying by another
     * oscillator with the frequency set to zero
    */
    SimpleOscillator operator*(const double) const;

    /**
     * Multiply the oscillator by another oscillator
    */
    SimpleOscillator operator*(const SimpleOscillator&) const;

    /**
     * Add another simple oscillator to the oscillator
     *
     * Note: This only works if the second oscillator has the same frequency as
     * the first. This is because mathematically the result is not a simple
     * oscillator if the frequencies differ. The code asserts if the frequencies
     * are not equal
    */
    SimpleOscillator operator+(const SimpleOscillator&) const;

    /**
     * Subtract one simple oscillator from another
     *
     * Note: This is implemented by adding -1 times the seconds oscillator to the
     * first. Hence, the oscillators must have the same frequencies for the same
     * reason as in the case of addition
    */
    SimpleOscillator operator-(const SimpleOscillator&) const;

    /**
     * Divide a simple oscillator by a constant
     *
     * Note: One can divide by a complex constant by dividing by a simple oscillator
     * with frequency set to zero
    */
    SimpleOscillator operator/(const double) const;

    /**
     * Divide a simple oscillator by another simple oscillator
    */
    SimpleOscillator operator/(const SimpleOscillator&) const;

    /**
     * Test another simple oscillator for equality
    */
   bool operator==(const SimpleOscillator&) const;

    /**
     * Test another simple oscillator for inequality
    */
   bool operator!=(const SimpleOscillator&) const;
 };

/**
 * Convenience function to multiply a constant double by a SimpleOscillator
*/
SimpleOscillator operator*(const double, const SimpleOscillator&);

/**
 * Convenience function to divide a constant double by a SimpleOscillator
*/
SimpleOscillator operator/(const double, const SimpleOscillator&);

/**
 * Convenince unary operator to negate a SimpleOscillator
*/
SimpleOscillator operator-(const SimpleOscillator&);

/**
 * Convenience stream operator
*/
std::ostream& operator<<(std::ostream&, const SimpleOscillator&);
