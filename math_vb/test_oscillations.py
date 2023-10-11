import numpy as np
import pytest

from math_vb.oscillations import SimpleOscillator

# Independent parameter defaults
A_DEFAULT = 5.5
OMEGA_DEFAULT = 2.0 * np.pi * 10.0
PHI_DEFAULT = 0.125 * np.pi

A_SECOND = 3.2
OMEGA_SECOND = 2.0 * np.pi * 7.0
PHI_SECOND = -0.25 * np.pi

# Parameters computed from the defaults
NUM_TIME_SAMPLES = 256
SAMPLE_RATE = 2.0 * np.pi / (8.0 * OMEGA_DEFAULT)  # 4 times the Nyquist
MAX_TIME = SAMPLE_RATE * (NUM_TIME_SAMPLES - 1)
TIMES_DEFAULT = np.linspace(0.0, MAX_TIME, NUM_TIME_SAMPLES)


@pytest.fixture
def default_so():
    return SimpleOscillator(
        A=A_DEFAULT,
        omega=OMEGA_DEFAULT,
        phi=PHI_DEFAULT,
    )


@pytest.fixture
def second_so():
    return SimpleOscillator(
        A=A_SECOND,
        omega=OMEGA_DEFAULT,
        phi=PHI_SECOND,
    )


@pytest.fixture
def third_so():
    return SimpleOscillator(
        A=A_SECOND,
        omega=OMEGA_SECOND,
        phi=PHI_SECOND,
    )


def test_simple_oscillator_call(default_so: SimpleOscillator):
    # Test that real and imaginary parts line up with what we'd expect
    signal = default_so(TIMES_DEFAULT)
    expected_real = A_DEFAULT * np.cos(OMEGA_DEFAULT * TIMES_DEFAULT + PHI_DEFAULT)
    expected_imag = A_DEFAULT * np.sin(OMEGA_DEFAULT * TIMES_DEFAULT + PHI_DEFAULT)
    assert np.allclose(signal.real, expected_real)
    assert np.allclose(signal.imag, expected_imag)


def test_simple_oscillator_negation(default_so: SimpleOscillator):
    new_so = -default_so
    assert new_so.A == -default_so.A
    assert new_so.omega == default_so.omega
    assert new_so.phi == default_so.phi


def test_simple_oscillator_addition(
    default_so: SimpleOscillator,
    second_so: SimpleOscillator,
    third_so: SimpleOscillator,
):
    # Test first that we get a ValueError for adding different frequencies
    with pytest.raises(ValueError):
        _ = default_so + third_so
    with pytest.raises(ValueError):
        _ = third_so + default_so

    # Test that the addition works as expected
    sum_so = default_so + second_so
    assert np.allclose(
        sum_so(TIMES_DEFAULT), default_so(TIMES_DEFAULT) + second_so(TIMES_DEFAULT)
    )


def test_simple_oscillator_multiplication(
    default_so: SimpleOscillator,
    second_so: SimpleOscillator,
    third_so: SimpleOscillator,
):
    # First check that scalar multiplication works
    scalar = 2.1
    expected_result = scalar * default_so(TIMES_DEFAULT)
    assert np.allclose((default_so * scalar)(TIMES_DEFAULT), expected_result)
    assert np.allclose((scalar * default_so)(TIMES_DEFAULT), expected_result)

    scalar = complex(1.1, 1.2)
    expected_result = scalar * default_so(TIMES_DEFAULT)
    assert np.allclose((default_so * scalar)(TIMES_DEFAULT), expected_result)
    assert np.allclose((scalar * default_so)(TIMES_DEFAULT), expected_result)

    # Now check that multiplication with second oscillator works
    expected_result = default_so(TIMES_DEFAULT) * second_so(TIMES_DEFAULT)
    assert np.allclose((default_so * second_so)(TIMES_DEFAULT), expected_result)
    assert np.allclose((second_so * default_so)(TIMES_DEFAULT), expected_result)

    # Now check that multiplication with third oscillator works
    expected_result = default_so(TIMES_DEFAULT) * third_so(TIMES_DEFAULT)
    assert np.allclose((default_so * third_so)(TIMES_DEFAULT), expected_result)
    assert np.allclose((third_so * default_so)(TIMES_DEFAULT), expected_result)

    # Check that multiplication raises if the type isn't scalar or simple oscillator
    scalar = "string"
    with pytest.raises(TypeError):
        _ = scalar * default_so
    with pytest.raises(TypeError):
        _ = default_so * scalar


def test_simple_oscillator_subtraction(
    default_so: SimpleOscillator,
    second_so: SimpleOscillator,
    third_so: SimpleOscillator,
):
    # Check that the subtraction operator works
    expected_result = default_so(TIMES_DEFAULT) - second_so(TIMES_DEFAULT)
    assert np.allclose((default_so - second_so)(TIMES_DEFAULT), expected_result)
    expected_result = second_so(TIMES_DEFAULT) - default_so(TIMES_DEFAULT)
    assert np.allclose((second_so - default_so)(TIMES_DEFAULT), expected_result)

    # Check that the subtraction operator raises with different frequencies
    with pytest.raises(ValueError):
        _ = default_so - third_so
    with pytest.raises(ValueError):
        _ = third_so - default_so


def test_simple_oscillator_division(
    default_so: SimpleOscillator,
    second_so: SimpleOscillator,
    third_so: SimpleOscillator,
):
    # Check that scalar division works
    scalar = 4.7
    expected_result = default_so(TIMES_DEFAULT) / scalar
    assert np.allclose((default_so / scalar)(TIMES_DEFAULT), expected_result)
    expected_result = scalar / default_so(TIMES_DEFAULT)
    assert np.allclose((scalar / default_so)(TIMES_DEFAULT), expected_result)

    scalar = complex(2.2, 0.7)
    expected_result = default_so(TIMES_DEFAULT) / scalar
    assert np.allclose((default_so / scalar)(TIMES_DEFAULT), expected_result)
    expected_result = scalar / default_so(TIMES_DEFAULT)
    assert np.allclose((scalar / default_so)(TIMES_DEFAULT), expected_result)

    # Check that division raises when the value is zero
    scalar = complex(0.0, 0.0)
    with pytest.raises(ValueError):
        _ = default_so / scalar

    zero_so = SimpleOscillator(A=0.0, omega=0.0, phi=0.0)
    with pytest.raises(ValueError):
        _ = default_so / zero_so

    # Check that division by a bad type raises
    scalar = "string"
    with pytest.raises(TypeError):
        _ = default_so / scalar
    with pytest.raises(TypeError):
        _ = scalar / default_so

    # Check that division by another SimpleOscillator works
    expected_result = default_so(TIMES_DEFAULT) / second_so(TIMES_DEFAULT)
    assert np.allclose((default_so / second_so)(TIMES_DEFAULT), expected_result)
    expected_result = second_so(TIMES_DEFAULT) / default_so(TIMES_DEFAULT)
    assert np.allclose((second_so / default_so)(TIMES_DEFAULT), expected_result)

    expected_result = default_so(TIMES_DEFAULT) / third_so(TIMES_DEFAULT)
    assert np.allclose((default_so / third_so)(TIMES_DEFAULT), expected_result)
    expected_result = third_so(TIMES_DEFAULT) / default_so(TIMES_DEFAULT)
    assert np.allclose((third_so / default_so)(TIMES_DEFAULT), expected_result)
