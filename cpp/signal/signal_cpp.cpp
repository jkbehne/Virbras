#include <tuple>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "signal/freeverb.hpp"

namespace py = pybind11;
using namespace std;

typedef tuple<vector<double>, vector<double>> OutputChannels;
typedef STLVectorInputSignal<double> InputType;
typedef STLVectorOutputSignal<double> OutputType;
typedef typename MIMOIIRFilter<double>::InputListType InputListType;
typedef typename MIMOIIRFilter<double>::OutputListType OutputListType;

OutputChannels freeverb_filter(
  vector<double> left_input,
  vector<double> right_input,
  const int num_transients,
  const int stereo_spread,
  const double dry,
  const double wet1,
  const double wet2,
  const double damp,
  const double reflect,
  const double g
)
{
  // Check that the inputs are the same size
  assert(left_input.size() == right_input.size());
  const int num_output_samples = left_input.size() + num_transients;

  // Make the filter
  auto filter = make_freeverb_filter<double>(stereo_spread, dry, wet1, wet2, damp, reflect, g);

  // Make the inputs
  const InputListType isignal_list {
    make_shared<InputType>(std::move(left_input)),
    make_shared<InputType>(std::move(right_input))
  };
  const OutputListType osignal_list {
    make_shared<OutputType>(num_output_samples),
    make_shared<OutputType>(num_output_samples)
  };

  // Run the filter
  filter.process(isignal_list, osignal_list, num_transients);

  // Get the outputs
  auto left_output = (dynamic_cast<const OutputType&>(*osignal_list[0])).output;
  auto right_output = (dynamic_cast<const OutputType&>(*osignal_list[1])).output;
  return make_tuple(std::move(left_output), std::move(right_output));
}

PYBIND11_MODULE(signal_cpp, m)
{
  m.def(
    "freeverb_filter",
    &freeverb_filter,
    py::arg("left_input"),
    py::arg("right_input"),
    py::arg("num_transients"),
    py::arg("stereo_spread") = 23,
    py::arg("dry") = 0.0,
    py::arg("wet1") = 1.0,
    py::arg("wet2") = 0.0,
    py::arg("damp") = 0.2,
    py::arg("reflect") = 0.84,
    py::arg("g") = 0.5
  );
}
