/**
 * This file contains the implementation of a simple model of a class-A triode
 * circuit. The design is taken from the book "Designing Audio Effects Plugins
 * in C++" by Will Pirkle. The chapter and section to reference is 19.12
*/
#pragma once

#include <optional>

#include "math/non_linear_tools.hpp"
#include "signal/analog_to_digital_filters.hpp"

namespace Signal {

/**
 * Basic static waveshaper implementation of a guitar tube amp triode
*/
template<typename ScalarType>
class TriodeClassAModel
{
  public: // Types
    typedef std::function<ScalarType(const ScalarType, const ScalarType)> NonLinearModel;

  public: // Members
    NonLinearModel nl_model;
    ScalarType saturation;
    ScalarType output_gain;
    std::optional<FirstOrderFilter<ScalarType>> high_pass_filter;
    std::optional<FirstOrderFilter<ScalarType>> low_shelf_filter;

    bool invert_output;

  public: // Methods
    TriodeClassAModel(
      NonLinearModel nl_model_,
      ScalarType saturation_,
      ScalarType output_gain_,
      std::optional<FirstOrderFilter<ScalarType>> high_pass_filter_,
      std::optional<FirstOrderFilter<ScalarType>> low_shelf_filter_,
      bool invert_output_
    ) : nl_model(nl_model_),
        saturation(saturation_),
        output_gain(output_gain_),
        high_pass_filter(high_pass_filter_),
        low_shelf_filter(low_shelf_filter_),
        invert_output(invert_output_) {}

    ScalarType next(const ScalarType x)
    {
      auto output = nl_model(x, saturation);
      if (invert_output) output *= -1.0;
      if (high_pass_filter) output = high_pass_filter->next(output);
      if (low_shelf_filter) output = low_shelf_filter->next(output);
      return output_gain * output;
    }
}

} // End namespace Signal
