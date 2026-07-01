



add_library(validator STATIC validator.c veval.c validator_bin_syntax.c)
target_link_libraries(validator PRIVATE  utils objects decoder) 
