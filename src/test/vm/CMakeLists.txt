
add_executable(tleb128 test_leb128.cpp)
target_link_libraries(tleb128 PRIVATE GTest::gtest_main aos_vm)

add_executable(tloader test_loader.cpp)
target_link_libraries(tloader PRIVATE GTest::gtest_main aos_vm)

include(GoogleTest)

# Finds all the Google tests associated with the executable
gtest_discover_tests(tleb128)
gtest_discover_tests(tloader)