


add_library(runtime STATIC runtime.c alloc.c)
target_link_libraries(runtime PRIVATE  objects utils validation )

