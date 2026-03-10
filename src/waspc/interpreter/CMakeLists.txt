



add_library(interpreter STATIC interpreter.c eval.c stack.c) 
target_link_libraries(interpreter PRIVATE  utils)
