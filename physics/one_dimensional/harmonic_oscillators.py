"""
This file is intended to provide basic tools for simple harmonic oscillation models.
"""
from abc import ABC, abstractmethod
import cmath
from dataclasses import dataclass
from math import sqrt
import numpy as np
from typing import Tuple


class BasicTimeBasedPhysicalSystem(ABC):
    """
    Abstract base class for physical systems in which displacement, velocity, and acceleration are
    of interest and only time is required as an external input for evaluation of each of
    these values. A cannocial example would be simple harmonic oscillators.
    """
    @abstractmethod
    def displacement(self, t: np.ndarray) -> np.ndarray:
        """Evaluate displacement of the system for an array of times

        Args:
            t (np.ndarray): Array of times to evaluate displacement at

        Returns:
            np.ndarray: System displacements evaluated at each time given
        """

    @abstractmethod
    def velocity(self, t: np.ndarray) -> np.ndarray:
        """Evaluate velocity of the system for an array of times

        Args:
            t (np.ndarray): Array of times to evaluate velocity at

        Returns:
            np.ndarray: System velocities evaluated at each time given
        """

    @abstractmethod
    def acceleration(self, t: np.ndarray) -> np.ndarray:
        """Evaluate acceleration of the system for an array of times

        Args:
            t (np.ndarray): Array of times to evaluate acceleration at

        Returns:
            np.ndarray: System accelerations evaluated at each time given
        """


@dataclass
class DampedHarmonicOscillator(BasicTimeBasedPhysicalSystem):
    """
    """
    k_spring: float
    mass: float
    dampening_factor: float
    omega0: float
    omegad: float
    alpha: float
    q_factor: float
    gamma_plus: complex
    gamma_minus: complex
    amplitude1: complex
    amplitude2: complex
    initial_displacement: complex
    initial_velocity: complex

    def __init__(
        self,
        k_spring: float,
        mass: float,
        dampening_factor: float,
        initial_displacement: float,
        initial_velocity: float,
    ):
        self.k_spring = k_spring
        self.mass = mass
        self.dampening_factor = dampening_factor
        self.initial_displacement = initial_displacement
        self.initial_velocity = initial_velocity
        self.omega0 = sqrt(k_spring / mass)
        self.omegad = cmath.sqrt(self.omega0**2 - self.alpha**2)
        self.alpha = dampening_factor / (2.0 * mass)
        self.q_factor = 0.5 * self.omega0 / self.alpha
        self.gamma_plus = -self.alpha + 1j * self.omegad
        self.gamma_minus = -self.alpha - 1j * self.omegad
        beta = (initial_velocity + self.alpha * initial_displacement) / self.omegad
        self.amplitude1 = 0.5 * (initial_displacement - 1j * beta)
        self.amplitude2 = 0.5 * (initial_displacement + 1j * beta)

    def _get_params(self) -> Tuple[complex, complex, complex, complex]:
        return (
            self.gamma_plus,
            self.gamma_minus,
            self.amplitude1,
            self.amplitude2,
        )

    def displacement(self, t: np.ndarray) -> np.ndarray:
        """Displacement function - do .real on the output for real initial conditions"""
        gamma_plus, gamma_minus, A1, A2 = self._get_params()
        return A1 * np.exp(gamma_plus * t) + A2 * np.exp(gamma_minus * t)

    def velocity(self, t: np.ndarray) -> np.ndarray:
        """Velocity function - do .real on the output for real initial conditions"""
        gamma_plus, gamma_minus, A1, A2 = self._get_params()
        return A1 * gamma_plus * np.exp(gamma_plus * t) + A2 * gamma_minus * np.exp(gamma_minus * t)

    def acceleration(self, t: np.ndarray) -> np.ndarray:
        """Acceleration function - do .real on the output for real initial conditions"""
        gamma_plus, gamma_minus, A1, A2 = self._get_params()
        gamma_plus2 = gamma_plus**2
        gamma_minus2 = gamma_minus**2
        return A1 * gamma_plus2 * np.exp(gamma_plus * t) + A2 * gamma_minus2 * np.exp(gamma_minus * t)

    def energy_loss_rate(self, t: np.ndarray) -> np.ndarray:
        """Get the energy loss rate for the system as a function of time

        Args:
            t (np.ndarray): The times to evaluate the energy loss rate at

        Returns:
            np.ndarray: The energy loss rate at the requested times
        """
        R = self.dampening_factor
        return -R * (self.velocity(t).real**2)
