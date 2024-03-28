/**
 * This file is intended to provide basic utilities for combining various
 * filtering primatives into series and parallel connections.
*/
#pragma once

#include <memory>
#include <variant>

#include <eigen3/Eigen/Dense>

#include "signal/feedback_comb_filter.hpp"
#include "signal/tapped_delay_line.hpp"

// Make some forward declarations
template<typename ScalarType>
class SeriesCombination;

template<typename ScalarType>
class ParallelCombination;

// Declare the variant for the currently offered computations
template<typename ScalarType>
struct FilterVariant
{
  typedef std::variant<
    TappedDelayLine<ScalarType>,
    FeedforwardFeedbackCombFilter<ScalarType>,
    FilteredFeedbackCombFilter<ScalarType>,
    SeriesCombination<ScalarType>,
    ParallelCombination<ScalarType>
  > type;
};

/**
 * Basic structure for running a series of filters, which are taken to be IIR filters,
 * though they don't necessarily have to be. Filter order matches the order of the
 * vector of computational units given during construction.
*/
template<typename ScalarType>
class SeriesCombination : public IIRFilter<ScalarType>
{
  public: // Types
    typedef std::shared_ptr<InputSignalStream<ScalarType>> InputType;
    typedef std::shared_ptr<OutputSignalStream<ScalarType>> OutputType;
    typedef typename FilterVariant<ScalarType>::type ComputationalUnitType;

  public: // Members
    std::vector<ComputationalUnitType> units;

  public: // Methods
    SeriesCombination(std::vector<ComputationalUnitType> units_) : units(std::move(units_)) {}

    /**
     * Compute the next output based on the input
     * 
     * This function takes the output of each computational unit in the chain and passes its
     * output to the next computational unit in the chain
    */
    virtual ScalarType next(const ScalarType input) override
    {
      auto output = input;
      for (auto iter = units.begin(); iter != units.end(); ++iter)
      {
        output = std::visit([output](auto&& unit){return unit.next(output);}, *iter);
      }
      return output;
    }
};

/**
 * Basic structure for running parallel filter computations, assuming a scalar output,
 * which is created by applying a linear transformation (using a vector) to the parallel
 * outputs.
*/
template<typename ScalarType>
class ParallelCombination : IIRFilter<ScalarType>
{
  public: // Types
    typedef std::shared_ptr<InputSignalStream<ScalarType>> InputType;
    typedef std::shared_ptr<OutputSignalStream<ScalarType>> OutputType;
    typedef typename FilterVariant<ScalarType>::type ComputationalUnitType;
    typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, 1> VectorType;

  public: // Members
    std::vector<ComputationalUnitType> units;
    const VectorType lt;

  public: // Methods
    /**
     * Sets the units, lt parameters and asserts that the lt is the right size
    */
    ParallelCombination(
      std::vector<ComputationalUnitType> units_,
      VectorType lt_
    ) : units(std::move(units_)), lt(std::move(lt_))
    {
      // Make sure that the linear transform is the right size
      assert(lt.rows() == units.size());
    }

    /**
     * Compute the next output from the current input
     * 
     * Output is computed by processing the input for each of the constituent computational
     * units, and then a single output is produced by performing a linear transform, given
     * by the lt class member (defined at construction)
    */
    ScalarType next(const ScalarType input)
    {
      VectorType output(units.size());
      for (int i = 0; i < units.size(); ++i)
      {
        output(i) = std::visit([input](auto&& unit){return unit.next(input);}, units[i]);
      }
      const auto m_output = lt.transpose() * output;
      return m_output(0, 0);
    }
};
