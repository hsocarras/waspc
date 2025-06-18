

add_executable(tvalidation test_wasm_decoder.cc test_wasm_validator.cc)
# Link the test executable with GoogleTest and the main 'waspc' library
target_link_libraries(tvalidation PRIVATE GTest::gtest_main waspc)

include(GoogleTest)

# Finds all the Google tests associated with the executable
gtest_discover_tests(tvalidation)
