



add_library(decoder STATIC decoder.c getter.c skipper.c type_check.c leb128.c ieee_754.c)
#target_link_libraries(decoder PRIVATE  utils )
