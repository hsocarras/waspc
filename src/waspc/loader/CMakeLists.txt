



add_library(loader STATIC wasm_loader.c wasm_decoder.c)
target_link_libraries(loader PRIVATE  utils )
