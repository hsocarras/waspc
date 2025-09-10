



add_library(validation STATIC wasm_validator.c wasm_decoder.c wasm_veval.c wasm_validator_buffer.c)
target_link_libraries(validation PRIVATE  utils )
