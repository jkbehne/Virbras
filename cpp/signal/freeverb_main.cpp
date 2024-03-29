#include <cstdlib>
#include <iostream>
#include <string>

#include "signal/freeverb.hpp"

using namespace std;

int main(int argc, char* argv[])
{
  // Set up the input args
  assert(argc == 3);
  const string ifile = argv[1];
  const string ofile = argv[2];

  // Set up freeverb filter constants
  const int stereo_spread = 0;
  const double dry = 0.5;
  const double wet1 = 0.035; // Not the right way to control clipping
  const double wet2 = 0.0;
  const double damp = 0.2;
  const double reflect = 0.84;
  const double g = 0.5;

  // Create freeverb filter
  auto filter = make_freeverb_filter(stereo_spread, dry, wet1, wet2, damp, reflect, g);

  // Setup the inputs
  DualChannelFileInput<double> dc_input(ifile, true);
  const typename MIMOIIRFilter<double>::InputListType isignal_list {
    dc_input.left_channel, dc_input.right_channel
  };

  // Get the overall output size
  const int num_inputs = dc_input.audio_file.getNumSamplesPerChannel();
  const int sample_rate = dc_input.audio_file.getSampleRate();
  const double extra_time = 2.0; // TODO: We may not want to hardcode this
  const int extra_samples = static_cast<int>(ceil(static_cast<double>(sample_rate) * extra_time));
  const int num_output_samples = num_inputs + extra_samples;

  // Setup the outputs
  DualChannelFileOutput<double> dc_output(
    sample_rate,
    dc_input.audio_file.getBitDepth(),
    num_output_samples
  );
  const typename MIMOIIRFilter<double>::OutputListType osignal_list {
    dc_output.left_out, dc_output.right_out
  };

  // Run the filter
  filter.process(isignal_list, osignal_list, extra_samples);

  // Write the output
  dc_output.write_final(ofile);
  return EXIT_SUCCESS;
}
