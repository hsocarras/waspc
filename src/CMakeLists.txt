#include path for header files
include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )      

#Main directory with webassembly runtime library
#add_subdirectory(waspc)

# Directory with basic driver to test waspc runtime.
add_subdirectory(driver)

#adding directory with tests source code
#add_subdirectory(test)