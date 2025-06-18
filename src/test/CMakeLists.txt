#include path for header files
include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/wasm
    )  

# BUILD_TESTING variable is created by include(CTest)
# It is set to ON by default
if (BUILD_TESTING)
    add_subdirectory(utils)
    add_subdirectory(runtime)
    add_subdirectory(validation)
endif()