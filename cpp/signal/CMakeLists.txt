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