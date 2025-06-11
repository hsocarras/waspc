# CMakeLists.txt for runtime unit tests
#
# This CMake configuration defines the build and test setup for the WASPC runtime unit tests.
#
# - Adds an executable target 'truntime' built from 'test_WpRuntime.cc'.
# - Links the test executable with GoogleTest (gtest_main) and the main 'waspc' library.
# - Includes GoogleTest CMake helpers.
# - Automatically discovers and registers all GoogleTest tests in the 'truntime' executable for use with CTest.
#
# Usage:
#   - Add new test source files to the add_executable() command as needed.
#   - Run 'ctest' or use your IDE's test explorer to execute the tests.
# CMakeLists.txt for runtime unit tests
#
# This CMake configuration defines the build and test setup for the WASPC runtime unit tests.
#
# - Adds an executable target 'truntime' built from 'test_WpRuntime.cc'.
# - Links the test executable with GoogleTest (gtest_main) and the main 'waspc' library.
# - Includes GoogleTest CMake helpers.
# - Automatically discovers and registers all GoogleTest tests in the 'truntime' executable for use with CTest.
#
# Usage:
#   - Add new test source files to the add_executable() command as needed.
#   - Run 'ctest' or use your IDE's test explorer to execute the tests.

add_executable(truntime test_WpRuntime.cc)
target_link_libraries(truntime PRIVATE GTest::gtest_main waspc)

include(GoogleTest)

# Finds all the Google tests associated with the executable
gtest_discover_tests(truntime)
