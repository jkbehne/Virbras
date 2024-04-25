cmake_minimum_required(VERSION 3.10)
project(Virbras)
enable_testing()

find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)

find_package(OpenMP REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)

add_library(standard_compiler_flags INTERFACE)
target_compile_features(standard_compiler_flags
    INTERFACE cxx_std_20
)

add_subdirectory(math)
add_subdirectory(signal)