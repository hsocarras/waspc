



add_library(decoder STATIC decoder.c getter.c skipper.c type_check.c )
target_link_libraries(decoder PRIVATE  utils )
