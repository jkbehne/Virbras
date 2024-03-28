/**
 * The intent of this file is to give some base classes to enable interact between FIR
 * and IIR filters with input and output signal streams of various kinds. The library
 * is generically templated, though for the most part the intended use case is real and
 * complex floating point scalars. An additional filter class is given for multi-input
 * multi-output (MIMO) IIR filters. Several reverb filter structures provide a use case
 * for this class. Although it is not an abstract class, its main intent is to provide
 * facilities for interacting with signal streams. Therefore, this file is a reasonable
 * place to store it for now.
*/
#pragma once

#include <algorithm>
#include <memory>
#include <optional>

#include <eigen3/Eigen/Dense>

#include "signal/signal_stream.hpp"

/**
 * Abstract base class intended to deal with FIR fitlers
 * 
 * The main reason for separating this out is that the number of signal transients
 * are known exactly if the length of the input signal is known. Hence, this class
 * handles processing streams differently in terms of the transients added to the
 * signal.
*/
template<typename ScalarType>
class FIRFilter
{
  public: // Types
    typedef std::shared_ptr<InputSignalStream<ScalarType>> InputType;
    typedef std::shared_ptr<OutputSignalStream<ScalarType>> OutputType;

  public: // Methods
    /**
     * Abstract method for computing the next output based on an input
    */
    virtual ScalarType next(const ScalarType) = 0;

    /**
     * Abstract method to calculate the number of transients
    */
    virtual const int max_delay() const = 0;

    /**
     * Process an entire input and write to the output until input is exhausted
     * 
     * Note: This function won't return if the input stream is a real-time
     * stream, unless the stream eventually fails to return a result.
     * 
     * Note: This function writes delay transients, which is the maximum delay
     * in terms of number of samples
    */
    void process(const InputType& isignal, const OutputType& osignal)
    {
      // Make sure pointers are valid
      assert(isignal != nullptr);
      assert(osignal != nullptr);

      const int num_transients = max_delay();

      // Read the input source until it's exhasuted
      while (true)
      {
        const auto input = isignal->read_next();
        if (not input) break; // Input stream is done
        osignal->write_next(next(*input));
      }

      // Use zeros as inputs to calculate the final transients
      for (int i = 0; i < num_transients; ++i)
      {
        osignal->write_next(next({}));
      }
    }
};

/**
 * Abstract base class intended to deal with IIR filters
*/
template<typename ScalarType>
class IIRFilter
{
  public: // Types
    typedef std::shared_ptr<InputSignalStream<ScalarType>> InputType;
    typedef std::shared_ptr<OutputSignalStream<ScalarType>> OutputType;

  public: // Methods
    /**
     * Abstract method for computing the next output based on an input
    */
    virtual ScalarType next(const ScalarType) = 0;

    /**
     * Process an entire input and write to the output until input is exhausted
     * 
     * Note: This function won't return if the input stream is a real-time
     * stream, unless the stream eventually fails to return a result.
     * 
     * Note: This function writes the number of specified delay transients,
     * although technically this is implementing an IIR filter.
    */
    void process(
      const InputType& isignal,
      const OutputType& osignal,
      const int num_output_transients
    )
    {
      // Make sure pointers are valid
      assert(isignal != nullptr);
      assert(osignal != nullptr);

      // Read the input source until it's exhasuted
      while (true)
      {
        const auto input = isignal->read_next();
        if (not input) break; // Input stream is done
        osignal->write_next(next(*input));
      }

      // Use zeros as inputs to calculate the final transients
      for (int i = 0; i < num_output_transients; ++i)
      {
        osignal->write_next(next({}));
      }
    }
};

/**
 * This class is intended to offer a common multi-input multi-output structure for
 * IIR filters. The basic idea is that we have N input streams that we wish to run
 * through N independent (though potentially different) IIR fitlers. The output is
 * then taken to be a scalar multiplied by the vector input summed with a constant
 * matrix scaling the filter outputs. This operation results in M outputs.
 * 
 * TODO: This isn't quite general MIMO. It works for the one use case, but we need
 * to add a matrix multiplying the input vector to make it truly multi-input
 * multi-output. Do this in the near future.
*/
template<typename ScalarType>
class MIMOIIRFilter
{
  public: // Types
    typedef std::shared_ptr<InputSignalStream<ScalarType>> InputType;
    typedef std::shared_ptr<OutputSignalStream<ScalarType>> OutputType;
    typedef std::vector<InputType> InputListType;
    typedef std::vector<OutputType> OutputListType;

    typedef std::shared_ptr<IIRFilter<ScalarType>> IIRFilterPtr;

    typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, Eigen::Dynamic> MatrixType;
    typedef Eigen::Matrix<ScalarType, Eigen::Dynamic, 1> VectorType;

  public: // Members
    const ScalarType input_scale;
    const MatrixType output_lt;
    const std::vector<IIRFilterPtr> filters;

  public: // Methods
    const int num_inputs() const {return filters.size();}
    const int num_outputs() const {return output_lt.rows();}

    /**
     * Set the input scale, output linear transform, and the constituent IIR filters
     * 
     * Note: This asserts that the output linear transform has the expected number of
     * columns.
     * 
     * Note: The output_lt sets the number of expected outputs
    */
    MIMOIIRFilter(
      const ScalarType input_scale_,
      MatrixType output_lt_,
      std::vector<IIRFilterPtr> filters_
    ) : input_scale(input_scale_), output_lt(std::move(output_lt_)), filters(std::move(filters_))
    {
      assert(output_lt.cols() == filters.size());
    }

    /**
     * Return the vector output from a vector input for the IIR filters given
     * 
     * Note: This leverages openmp for parallel computation
    */
    const VectorType next(const VectorType& input) const
    {
      VectorType out(input.rows());
      #pragma omp parallel for
      for (int i = 0; i < filters.size(); ++i)
      {
        out(i) = filters[i]->next(input(i));
      }
      return input_scale * input + output_lt * out;
    }

    /**
     * Process an entire input and write to the output until input is exhausted
     * 
     * Note: This function won't return if the input stream is a real-time
     * stream, unless the stream eventually fails to return a result.
     * 
     * Note: This function writes the number of specified delay transients,
     * although technically this is implementing an IIR filter.
    */
    void process(
      const InputListType& isignal_list,
      const OutputListType& osignal_list,
      const int num_output_transients
    ) const
    {
      // Make sure we have the right number of input / output streams
      assert(isignal_list.size() == num_inputs());
      assert(osignal_list.size() == num_outputs());

      // Make sure none of the pointers are null
      std::for_each(
        isignal_list.begin(),
        isignal_list.end(),
        [](auto&& arg){assert(arg != nullptr);}
      );
      std::for_each(
        osignal_list.begin(),
        osignal_list.end(),
        [](auto&& arg){assert(arg != nullptr);}
      );

      // Read the input sources until they're exhasuted (should be the same length)
      while (true)
      {
        const auto input = read_vector(isignal_list);
        if (not input) break; // Input is exhasuted
        write_vector(osignal_list, next(*input));
      }

      // Use zeros as inputs to calculate the final transients
      for (int i = 0; i < num_output_transients; ++i)
      {
        write_vector(osignal_list, next(VectorType::Zero(num_inputs())));
      }
    }

  private: // Methods
    /**
     * Convenience function to read the input signal list out as a vector
     * 
     * Note: If any stream in the input list fails to return a result, then it is
     * asserted that all other input streams fail to return a result as well. If this
     * is the case, then the function returns a "none" result as well.
    */
    std::optional<VectorType> read_vector(const InputListType& isignal_list) const
    {
      const auto n_inputs = num_inputs();
      VectorType input(n_inputs);
      bool input_end = false;
      for (int i = 0; i < n_inputs; ++i)
      {
        if (not input_end)
        {
          // Still expecting to read inputs
          const auto in = isignal_list[i]->read_next();
          if (not in)
          {
            assert(i == 0); // No previous inputs succeeded
            input_end = true;
            continue;
          }
          input(i) = *in;
        }
        else
        {
          // Input is expected to be done
          assert(not isignal_list[i]->read_next());
        }
      }
      if (input_end) return {};
      return input;
    }

    /**
     * Convenience function to write the output vector to the output list
     * 
     * Note: This happens in parallel
    */
    void write_vector(const OutputListType& osignal_list, const VectorType& output) const
    {
      #pragma omp parallel for
      for (int i = 0; i < num_outputs(); ++i)
      {
        osignal_list[i]->write_next(output(i));
      }
    }
};
