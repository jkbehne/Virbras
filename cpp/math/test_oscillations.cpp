#define BOOST_TEST_MODULE test_oscillations
#include <boost/test/included/unit_test.hpp>

#include "math/eigen_tools.hpp"
#include "math/oscillations.hpp"

using namespace EigenTools;

struct SimpleOscillatorFixture
{
  public: // Members
    const SimpleOscillator so1;
    const SimpleOscillator so2;
    const SimpleOscillator so3;
    const Eigen::Vector4d times;
    const double scale;

  public: // Methods
    SimpleOscillatorFixture() : so1(SimpleOscillator(2.0, 5.0, 0.1)),
                                so2(SimpleOscillator(4.0, 5.0, -0.5)),
                                so3(SimpleOscillator(1.0, 1.0, 1.0)),
                                times({0.0, 0.1, 0.2, 0.3}),
                                scale(0.25) {}
    ~SimpleOscillatorFixture() {}
};

const double tolerance = 1e-10;
bool is_close(const double a, const double b)
{
  return abs(a - b) < tolerance;
}

BOOST_FIXTURE_TEST_CASE( test_equality, SimpleOscillatorFixture )
{
  // Test the equality operator
  BOOST_TEST(so1 == so1);
  BOOST_TEST(so2 == so2);
  BOOST_TEST(so3 == so3);

  // Test the inequality operator
  const auto so0 = SimpleOscillator(0.0, 0.0, 0.0);
  BOOST_TEST(so1 != so0);
  BOOST_TEST(so2 != so0);
  BOOST_TEST(so3 != so0);
}

BOOST_FIXTURE_TEST_CASE( test_call, SimpleOscillatorFixture )
{
  const auto result1 = so1(times);
  BOOST_TEST(result1.size() == times.size());
  const auto result2 = so2(times);
  BOOST_TEST(result2.size() == times.size());
  const auto result3 = so3(times);
  BOOST_TEST(result3.size() == times.size());
}

BOOST_FIXTURE_TEST_CASE( test_scalar_mul, SimpleOscillatorFixture )
{
  // Test scalar multiplication
  const auto expected1 = SimpleOscillator(scale * so1.A, so1.omega, so1.phi);
  const auto scaled1 = scale * so1;
  BOOST_TEST(scaled1 == expected1);

  const auto expected2 = SimpleOscillator(scale * so2.A, so2.omega, so2.phi);
  const auto scaled2 = scale * so2;
  BOOST_TEST(scaled2 == expected2);

  const auto expected3 = SimpleOscillator(scale * so3.A, so3.omega, so3.phi);
  const auto scaled3 = scale * so3;
  BOOST_TEST(scaled3 == expected3);

  // Test reverse scalar multiplication
  const auto rscaled1 = so1 * scale;
  BOOST_TEST(rscaled1 == expected1);

  const auto rscaled2 = so2 * scale;
  BOOST_TEST(rscaled2 == expected2);

  const auto rscaled3 = so3 * scale;
  BOOST_TEST(rscaled3 == expected3);
}

BOOST_FIXTURE_TEST_CASE( test_scalar_division, SimpleOscillatorFixture )
{
  // Test scalar division
  const auto expected1 = SimpleOscillator(so1.A / scale, so1.omega, so1.phi);
  const auto div1 = so1 / scale;
  BOOST_TEST(div1 == expected1);

  const auto expected2 = SimpleOscillator(so2.A / scale, so2.omega, so2.phi);
  const auto div2 = so2 / scale;
  BOOST_TEST(div2 == expected2);

  const auto expected3 = SimpleOscillator(so3.A / scale, so3.omega, so3.phi);
  const auto div3 = so3 / scale;
  BOOST_TEST(div3 == expected3);

  // Test reverse scalar division
  const auto rexpected1 = SimpleOscillator(scale / so1.A, -so1.omega, -so1.phi);
  const auto rdiv1 = scale / so1;
  BOOST_TEST(rdiv1 == rexpected1);

  const auto rexpected2 = SimpleOscillator(scale / so2.A, -so2.omega, -so2.phi);
  const auto rdiv2 = scale / so2;
  BOOST_TEST(rdiv2 == rexpected2);

  const auto rexpected3 = SimpleOscillator(scale / so3.A, -so3.omega, -so3.phi);
  const auto rdiv3 = scale / so3;
  BOOST_TEST(rdiv3 == rexpected3);
}

BOOST_FIXTURE_TEST_CASE( test_addition, SimpleOscillatorFixture )
{
  // Test summing oscillators with themselves
  const auto self_sum1 = so1 + so1;
  const auto tso1 = 2.0 * so1;
  BOOST_TEST(is_close(self_sum1.A, tso1.A));
  BOOST_TEST(self_sum1.omega == tso1.omega);
  BOOST_TEST(is_close(self_sum1.phi, tso1.phi));

  const auto self_sum2 = so2 + so2;
  const auto tso2 = 2.0 * so2;
  BOOST_TEST(is_close(self_sum2.A, tso2.A));
  BOOST_TEST(self_sum2.omega == tso2.omega);
  BOOST_TEST(is_close(self_sum2.phi, tso2.phi));

  const auto self_sum3 = so3 + so3;
  const auto tso3 = 2.0 * so3;
  BOOST_TEST(self_sum3.A, tso3.A);
  BOOST_TEST(self_sum3.omega == tso3.omega);
  BOOST_TEST(is_close(self_sum3.phi, tso3.phi));

  // Test summing the oscillators that are compatible
  const auto sum = so1 + so2;
  const auto sum_response = sum(times);
  const VectorXcd expected = so1(times) + so2(times);
  BOOST_TEST(all_close(sum_response, expected, tolerance));
}

BOOST_FIXTURE_TEST_CASE( test_subtraction, SimpleOscillatorFixture )
{
  // Test subtracting oscillators from themselves
  const auto sub1 = so1 - so1;
  BOOST_TEST(is_close(sub1.A, 0.0));
  BOOST_TEST(sub1.omega == so1.omega);
  BOOST_TEST(is_close(sub1.phi, 0.0));

  const auto sub2 = so2 - so2;
  BOOST_TEST(is_close(sub2.A, 0.0));
  BOOST_TEST(sub2.omega == so2.omega);
  BOOST_TEST(is_close(sub2.phi, 0.0));

  const auto sub3 = so3 - so3;
  BOOST_TEST(is_close(sub3.A, 0.0));
  BOOST_TEST(sub3.omega == so3.omega);
  BOOST_TEST(is_close(sub3.phi, 0.0));

  // Test subtracting the oscillators that are compatible
  const auto sub12 = so1 - so2;
  const VectorXcd response12 = sub12(times);
  const VectorXcd expected12 = so1(times) - so2(times);
  BOOST_TEST(all_close(response12, expected12, tolerance));

  const auto sub21 = so2 - so1;
  const VectorXcd response21 = sub21(times);
  const VectorXcd expected21 = so2(times) - so1(times);
  BOOST_TEST(all_close(response21, expected21, tolerance));
}

BOOST_FIXTURE_TEST_CASE( test_multiplication, SimpleOscillatorFixture )
{
  // Check that multiplication result is commutative
  BOOST_TEST(so1 * so2 == so2 * so1);
  BOOST_TEST(so2 * so3 == so3 * so2);
  BOOST_TEST(so1 * so3 == so3 * so1);

  // Test self-multiplications
  const auto smul1 = so1 * so1;
  const VectorXcd sresponse1 = smul1(times);
  const VectorXcd sexpected1 = so1(times).cwiseProduct(so1(times));
  BOOST_TEST(all_close(sresponse1, sexpected1, tolerance));

  const auto smul2 = so2 * so2;
  const VectorXcd sresponse2 = smul2(times);
  const VectorXcd sexpected2 = so2(times).cwiseProduct(so2(times));
  BOOST_TEST(all_close(sresponse2, sexpected2, tolerance));

  const auto smul3 = so3 * so3;
  const VectorXcd sresponse3 = smul3(times);
  const VectorXcd sexpected3 = so3(times).cwiseProduct(so3(times));
  BOOST_TEST(all_close(sresponse3, sexpected3, tolerance));

  // Test other combinations
  const auto mul12 = so1 * so2;
  const VectorXcd response12 = mul12(times);
  const VectorXcd expected12 = so1(times).cwiseProduct(so2(times));
  BOOST_TEST(all_close(response12, expected12, tolerance));

  const auto mul13 = so1 * so3;
  const VectorXcd response13 = mul13(times);
  const VectorXcd expected13 = so1(times).cwiseProduct(so3(times));
  BOOST_TEST(all_close(response13, expected13, tolerance));

  const auto mul23 = so2 * so3;
  const VectorXcd response23 = mul23(times);
  const VectorXcd expected23 = so2(times).cwiseProduct(so3(times));
  BOOST_TEST(all_close(response23, expected23, tolerance));
}

BOOST_FIXTURE_TEST_CASE( test_division, SimpleOscillatorFixture )
{
  // Test that we get a constant ones function in self division
  const auto self_expected = SimpleOscillator(1.0, 0.0, 0.0);
  const auto sdiv1 = so1 / so1;
  BOOST_TEST(sdiv1 == self_expected);
  const auto sdiv2 = so2 / so2;
  BOOST_TEST(sdiv2 == self_expected);
  const auto sdiv3 = so3 / so3;
  BOOST_TEST(sdiv3 == self_expected);

  // Test the other combinations
  const auto div12 = so1 / so2;
  const VectorXcd expected12 = so1(times).cwiseQuotient(so2(times));
  BOOST_TEST(all_close(div12(times), expected12, tolerance));

  const auto div21 = so2 / so1;
  const VectorXcd expected21 = so2(times).cwiseQuotient(so1(times));
  BOOST_TEST(all_close(div21(times), expected21, tolerance));

  const auto div13 = so1 / so3;
  const VectorXcd expected13 = so1(times).cwiseQuotient(so3(times));
  BOOST_TEST(all_close(div13(times), expected13, tolerance));

  const auto div31 = so3 / so1;
  const VectorXcd expected31 = so3(times).cwiseQuotient(so1(times));
  BOOST_TEST(all_close(div31(times), expected31, tolerance));

  const auto div23 = so2 / so3;
  const VectorXcd expected23 = so2(times).cwiseQuotient(so3(times));
  BOOST_TEST(all_close(div23(times), expected23, tolerance));

  const auto div32 = so3 / so2;
  const VectorXcd expected32 = so3(times).cwiseQuotient(so2(times));
  BOOST_TEST(all_close(div32(times), expected32, tolerance));
}
