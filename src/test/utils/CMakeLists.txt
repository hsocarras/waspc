# CMakeLists.txt for utils unit tests
#
# This CMake configuration defines the build and test setup for the WASPC utils unit tests.
#
# - Adds an executable target 'tutils' built from 'test_leb128.cc' and 'test_hash_table.cc'.
# - Links the test executable with GoogleTest (gtest_main) and the main 'waspc' library.
# - Includes GoogleTest CMake helpers.
# - Automatically discovers and registers all GoogleTest tests in the 'tutils' executable for use with CTest.
#
# Usage:
#   - Add new test source files to the add_executable() command as needed.
#   - Run 'ctest' or use your IDE's test explorer to execute the tests.
# CMakeLists.txt for utils unit tests
#
# This CMake configuration defines the build and test setup for the WASPC utils unit tests.
#
# - Adds an executable target 'tutils' built from 'test_leb128.cc' and 'test_hash_table.cc'.
# - Links the test executable with GoogleTest (gtest_main) and the main 'waspc' library.
# - Includes GoogleTest CMake helpers.
# - Automatically discovers and registers all GoogleTest tests in the 'tutils' executable for use with CTest.
#
# Usage:
#   - Add new test source files to the add_executable() command as needed.
#   - Run 'ctest' or use your IDE's test explorer to execute the tests.

add_executable(tutils test_leb128.cc test_hash_table.cc)
target_link_libraries(tutils PRIVATE GTest::gtest_main waspc)

include(GoogleTest)

# Finds all the Google tests associated with the executable
gtest_discover_tests(tutils)
