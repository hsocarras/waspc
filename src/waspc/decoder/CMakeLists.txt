



add_library(decoder STATIC wasm_decoder.c )
target_link_libraries(decoder PRIVATE  utils )
