add_executable(test_signal_stream test_signal_stream.cpp)
target_include_directories(test_signal_stream
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_link_libraries(test_signal_stream PUBLIC standard_compiler_flags)
add_test(NAME test_signal_stream COMMAND test_signal_stream)

add_executable(test_tapped_delay_line test_tapped_delay_line.cpp)
target_include_directories(test_tapped_delay_line
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_link_libraries(test_tapped_delay_line PUBLIC standard_compiler_flags)
add_test(NAME test_tapped_delay_line COMMAND test_tapped_delay_line)

add_executable(test_feedback_comb_filter test_feedback_comb_filter.cpp)
target_include_directories(test_feedback_comb_filter
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_link_libraries(test_feedback_comb_filter PUBLIC standard_compiler_flags)
add_test(NAME test_feedback_comb_filter COMMAND test_feedback_comb_filter)

add_executable(test_freeverb test_freeverb.cpp)
target_include_directories(test_freeverb
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_link_libraries(test_freeverb
    PUBLIC standard_compiler_flags
    PRIVATE OpenMP::OpenMP_CXX
)
add_test(NAME test_freeverb COMMAND test_freeverb)

add_executable(test_utilities test_utilities.cpp)
target_include_directories(test_utilities
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_link_libraries(test_utilities PUBLIC standard_compiler_flags)
add_test(NAME test_utilities COMMAND test_utilities)

add_executable(test_analog_to_digital_filters test_analog_to_digital_filters.cpp)
target_include_directories(test_analog_to_digital_filters
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_link_libraries(test_analog_to_digital_filters PUBLIC standard_compiler_flags)
add_test(NAME test_analog_to_digital_filters COMMAND test_analog_to_digital_filters)

add_executable(test_fft_processing test_fft_processing.cpp)
target_include_directories(test_fft_processing
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
    PRIVATE PkgConfig::FFTW
)
target_link_libraries(test_fft_processing
    PUBLIC standard_compiler_flags
    PRIVATE PkgConfig::FFTW
)

pybind11_add_module(signal_cpp signal_cpp.cpp)
target_include_directories(signal_cpp
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/usr/local/include/eigen3"
    PUBLIC "/Library/Frameworks/Python.framework/Versions/3.12/include/python3.12/"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_link_libraries(signal_cpp
    PUBLIC standard_compiler_flags
)

add_executable(freeverb_main freeverb_main.cpp)
target_include_directories(freeverb_main
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include"
    PUBLIC "/Users/josh/Virbras/cpp"
)
target_link_libraries(freeverb_main
    PUBLIC standard_compiler_flags
    PRIVATE OpenMP::OpenMP_CXX
)