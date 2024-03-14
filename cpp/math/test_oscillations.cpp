#define BOOST_TEST_MODULE test_oscillations
#include <boost/test/included/unit_test.hpp>
#include "/Users/josh/Virbras/cpp/math/oscillations.hpp"

struct SimpleOscillatorFixture {
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

BOOST_FIXTURE_TEST_CASE( test_call, SimpleOscillatorFixture )
{
  const auto result1 = so1(times);
  BOOST_TEST(result1.size() == times.size());
  const auto result2 = so2(times);
  BOOST_TEST(result2.size() == times.size());
  const auto result3 = so3(times);
  BOOST_TEST(result3.size() == times.size());
}
