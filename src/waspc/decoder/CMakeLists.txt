



add_library(decoder STATIC wasm_loader.c wasm_decoder.c)
target_link_libraries(decoder PRIVATE  utils )
