

add_executable(tdecoder test1_wasm_decoder.cc test2_wasm_decoder.cc test1_skipper.cc)
# Link the test executable with GoogleTest and the main 'waspc' library
target_link_libraries(tdecoder PRIVATE GTest::gtest_main decoder wasm_file_reader)

include(GoogleTest)

# Finds all the Google tests associated with the executable
gtest_discover_tests(tdecoder)
