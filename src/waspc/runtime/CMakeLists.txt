


add_library(runtime STATIC runtime.c store.c)
target_link_libraries(runtime PRIVATE  objects utils validation interpreter)

