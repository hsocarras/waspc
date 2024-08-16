



add_library(runtime STATIC init_runtime.c)
target_link_libraries(runtime PRIVATE memory)
