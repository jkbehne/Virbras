from dataclasses import dataclass
from math import atan2, cos, sin, sqrt
from numbers import Number
from typing import Protocol, Union

import numpy as np


@dataclass
class SimpleOscillator:
    """
    This class is intended to represent simple oscillators which have the
    following one-dimensional equation: x(t) = Ae^{j(omega t + phi)}.
    """

    A: Number
    omega: float
    phi: float

    def __call__(self, t: np.ndarray) -> np.ndarray:
        """Evaluate the oscillator at an array of variable values (often time values)

        Args:
            t (np.ndarray): The array of variable values to evaluate

        Returns:
            np.ndarray: The value of the oscillator at each input value
        """
        A, omega, phi = self.A, self.omega, self.phi
        return A * np.exp(1j * (omega * t + phi))

    def __neg__(self) -> "SimpleOscillator":
        """Negation operator for simple oscillators

        Returns:
            SimpleOscillator: The negated simple oscillator
        """
        return SimpleOscillator(
            A=-1.0 * self.A,
            omega=self.omega,
            phi=self.phi,
        )

    def __add__(self, other: "SimpleOscillator") -> "SimpleOscillator":
        """Add two simple oscilators of the same frequency

        Args:
            other (SimpleOscillator): The SimpleOscillator to add to the current one

        Raises:
            ValueError: If the SimpleOscillators are not the same frequency, they will not result in
            another SimpleOscillators. In this case, call add_so_different_frequency instead, as that
            will return an oscillator that is non-simple, i.e., has multiple frequencies.

        Returns:
            SimpleOscillator: The simple oscillator that results from the addition
        """
        if self.omega != other.omega:
            raise ValueError(
                "Adding simple oscillators of different frequencies does not result in a simple oscillator! "
                f"({self.omega} != {other.omega}). Use add_so_different_frequency instead."
            )

        cos_sum = self.A * cos(self.phi) + other.A * cos(other.phi)
        cos_sum2 = cos_sum**2
        sin_sum = self.A * sin(self.phi) + other.A * sin(other.phi)
        sin_sum2 = sin_sum**2
        return SimpleOscillator(
            A=sqrt(cos_sum2 + sin_sum2), omega=self.omega, phi=atan2(sin_sum, cos_sum)
        )

    def __mul__(self, other: Union[Number, "SimpleOscillator"]) -> "SimpleOscillator":
        """Multiply by either a scalar or another simple oscillator

        Args:
            other (Union[float, SimpleOscillator]): The multiplier (can be Number or SimpleOscillator)

        Raises:
            TypeError: If the type of other is not a Number or SimpleOscillator

        Returns:
            SimpleOscillator: An oscillator representing the mathematical result of multiplication
        """
        if isinstance(other, Number):
            return SimpleOscillator(
                A=self.A * other,
                omega=self.omega,
                phi=self.phi,
            )
        elif isinstance(other, SimpleOscillator):
            return SimpleOscillator(
                A=self.A * other.A,
                omega=self.omega + other.omega,
                phi=self.phi + other.phi,
            )
        else:
            raise TypeError(f"Cannot multiply SimpleOscillator by type: {type(other)}")

    def __rmul__(self, other: Union[Number, "SimpleOscillator"]) -> "SimpleOscillator":
        return self.__mul__(other)

    def __sub__(self, other: "SimpleOscillator") -> "SimpleOscillator":
        """Subtract another simple oscillator

        Args:
            other (SimpleOscillator): The simple oscillator to subtract

        Returns:
            SimpleOscillator: The mathematical (simple oscillator) result of subtraction
        """
        return self.__add__(-other)

    def __truediv__(
        self, other: Union[Number, "SimpleOscillator"]
    ) -> "SimpleOscillator":
        """Divide a simple oscillator by a scalar or another simple oscillator

        Args:
            other (Union[float, SimpleOscillator]): The scalar or simple oscillator to divide by

        Raises:
            ValueError: If other is a Number equal to zero
            ValueError: If other is a SimpleOscillator with zero amplitude
            TypeError: If other is not a SimpleOscillator or float

        Returns:
            SimpleOscillator: The mathematical result of division
        """
        if isinstance(other, Number):
            if other == 0.0:
                raise ValueError("Cannot divide SimpleOscillator by zero")
            return self.__mul__(1.0 / other)
        elif isinstance(other, SimpleOscillator):
            if other.A == 0.0:
                raise ValueError(
                    "Cannot divide SimpleOscillator by SimpleOscillator with zero amplitude"
                )
            return self.__mul__(
                SimpleOscillator(
                    A=1.0 / other.A,
                    omega=-other.omega,
                    phi=-other.phi,
                )
            )
        else:
            raise TypeError(
                f"Division of SimpleOscillator by type: {type(other)} not defined"
            )

    def __rtruediv__(self, other: Number) -> "SimpleOscillator":
        if not isinstance(other, Number):
            raise TypeError(f"__rdiv__ should not be called with type: {type(other)}")
        return SimpleOscillator(
            A=other,
            omega=0.0,
            phi=0.0,
        ).__truediv__(self)
