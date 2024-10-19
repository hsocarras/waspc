
add_executable(tleb128 test_leb128.cpp)
target_link_libraries(tleb128 PRIVATE GTest::gtest_main waspc)

include(GoogleTest)

# Finds all the Google tests associated with the executable
gtest_discover_tests(tleb128)
