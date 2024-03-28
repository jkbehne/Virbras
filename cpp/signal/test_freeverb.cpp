#define BOOST_TEST_MODULE test_freeverb
#include <boost/test/included/unit_test.hpp>

#include "signal/freeverb.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( test_freeverb )
{
  // Setup the default constants
  const int stereo_spread = 23;
  const double dry = 0.0;
  const double wet1 = 1.0;
  const double wet2 = 0.0;
  const double damp = 0.2;
  const double reflect = 0.84;
  const double g = 0.5;

  // Construct the freeverb filter
  auto filter = make_freeverb_filter(stereo_spread, dry, wet1, wet2, damp, reflect, g);

  // Construct the inputs and outputs
  typedef typename MIMOIIRFilter<double>::InputListType InputListType;
  typedef typename MIMOIIRFilter<double>::OutputListType OutputListType;

  const InputListType isignal_list {
    make_shared<STLVectorInputSignal<double>>(
      vector<double> {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0}
    ),
    make_shared<STLVectorInputSignal<double>>(
      vector<double> {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0}
    )
  };
  const int num_transients = 200;
  const OutputListType osignal_list {
    make_shared<STLVectorOutputSignal<double>>(num_transients + 8),
    make_shared<STLVectorOutputSignal<double>>(num_transients + 8)
  };

  filter.process(isignal_list, osignal_list, num_transients);
  const auto output0 = (dynamic_cast<const STLVectorOutputSignal<double>&>(*osignal_list[0])).output;
  const auto output1 = (dynamic_cast<const STLVectorOutputSignal<double>&>(*osignal_list[1])).output;
  BOOST_TEST(output0.size() == 208);
  BOOST_TEST(output1.size() == 208);
}
