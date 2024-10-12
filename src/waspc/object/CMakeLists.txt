



add_library(object STATIC result.c error.c wasm_module.c wasm_bin_module.c)
#target_link_libraries(object PRIVATE diagnostic utils memory)
