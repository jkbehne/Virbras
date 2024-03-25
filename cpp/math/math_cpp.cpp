/**
 * This file binds the functions in the math directory into a single, compiled
 * library (which obviously has to be built). The functions can then be called
 * naturally from Python.
*/
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

#include "math/oscillations.hpp"

namespace py = pybind11;

PYBIND11_MODULE(math_cpp, m)
{
  py::class_<SimpleOscillator>(m, "SimpleOscillator")
    .def(
      py::init<const double, const double, const double>(),
      py::arg("A"),
      py::arg("omega"),
      py::arg("phi"),
      "Construct object representing x(t) = A * exp(j * (omega * t + phi))"
    )
    .def_readonly("A", &SimpleOscillator::A)
    .def_readonly("omega", &SimpleOscillator::omega)
    .def_readonly("phi", &SimpleOscillator::phi)
    .def(
      "__repr__",
      [](const SimpleOscillator& so)
      {
        return "SimpleOscillator(A=" + std::to_string(so.A) + ", omega=" + std::to_string(so.omega)
          + ", phi=" + std::to_string(so.phi) + ")";
      }
    )
    .def(
      "__call__",
      &SimpleOscillator::operator(),
      py::arg("times"),
      "Evaluate the simple oscillator at a 1-D array of times (result is complex)"
    )
    .def(
      "__neg__",
      &operator-,
      "Negate a simple oscillator (flips the sign on A)"
    )
    .def(
      "__mul__",
      static_cast<SimpleOscillator (SimpleOscillator::*) (const double) const>(&SimpleOscillator::operator*),
      "Multipy the oscillator by a constant, real value (use oscillator with omega=0 for complex)"
    )
    .def(
      "__mul__",
      static_cast<SimpleOscillator (SimpleOscillator::*) (const SimpleOscillator&) const>(&SimpleOscillator::operator*),
      "Multiply a simple oscillator by another simple oscillator"
    )
    .def(
      "__rmul__",
      [](const SimpleOscillator& so, const double scalar) { return scalar * so; },
      "Multiply a real scalar by a simple oscillator (results in an oscillator)"
    )
    .def(
      "__add__",
      &SimpleOscillator::operator+,
      "Add two simple oscillators (frequencies MUST be equal)"
    )
    .def(
      "__sub__",
      &SimpleOscillator::operator-,
      "Subtract one simple oscillator from another (frequencies MUST be equal)"
    )
    .def(
      "__truediv__",
      static_cast<SimpleOscillator (SimpleOscillator::*) (const double) const>(&SimpleOscillator::operator/),
      "Divide an oscillator by a real constant (use oscillator with omega=0 for complex)"
    )
    .def(
      "__truediv__",
      static_cast<SimpleOscillator (SimpleOscillator::*) (const SimpleOscillator&) const>(&SimpleOscillator::operator/),
      "Divide one simple oscillator by another"
    )
    .def(
      "__rtruediv__",
      [](const SimpleOscillator& so, const double scalar){ return scalar / so; },
      "Divide a real constant by an oscillator (use oscillator with omega=0 for complex)"
    );
}
