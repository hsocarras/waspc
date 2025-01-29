



add_library(validation STATIC wasm_validator.c wasm_decoder.c)
target_link_libraries(validation PRIVATE  utils )
