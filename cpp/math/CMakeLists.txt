add_library(math oscillations.cpp)
pybind11_add_module(math_cpp math_cpp.cpp)

target_include_directories(math
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_include_directories(math_cpp
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/usr/local/include/eigen3"
    PUBLIC "/Library/Frameworks/Python.framework/Versions/3.12/include/python3.12/"
    PUBLIC "/Users/josh/Virbras/cpp"
)

target_link_libraries(math PUBLIC standard_compiler_flags)
target_link_libraries(math_cpp
    PUBLIC standard_compiler_flags
    PUBLIC math
)

add_executable(test_oscillations test_oscillations.cpp)
target_link_libraries(test_oscillations PUBLIC math)
add_test(NAME test_oscillations COMMAND test_oscillations)

add_executable(test_eigen_tools test_eigen_tools.cpp)
target_link_libraries(test_eigen_tools PUBLIC math)
add_test(NAME test_eigen_tools COMMAND test_eigen_tools)