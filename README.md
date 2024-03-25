# Virbras
Virbras is a package for musical instrument modeling based on physical acoustics and related models. As such, this package contains many general purpose
functions for modeling acoustics. There is also some associated signal processing support. The overall idea is to provide many acoustics and signal processing
primatives in C++, but to include bindings to Python for convenience, ease of visualization, etc.

# Dependencies
This package uses CMake as a build system (targeting the C++20 standard). It uses the open source libraries Eigen, PyBind11, and Boost open source libraries. 
Currently, all dependencies are header-only, although that could change in the future with regard to Boost. The Python part of the library assumes only basic
dependencies such as NumPy and SciPy.

# Current State
At this point, this repository serves more as a project intended to revitalize the author's skills in terms of the CMake, modern C++, Eigen, PyBind11, and Boost 
technology stack. In the future, this may change, although that depends on the amount of ongoing effort the author is able to apply to this project.
