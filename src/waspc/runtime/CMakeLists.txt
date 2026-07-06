


file(GLOB_RECURSE RUNTIME_BUILDIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/buildin/**.c"
)

add_library(runtime STATIC runtime.c ${RUNTIME_BUILDIN_SOURCES})
target_link_libraries(runtime PRIVATE  objects utils memory decoder interpreter validator)

