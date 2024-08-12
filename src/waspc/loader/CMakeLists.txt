



add_library(loader STATIC wasm_loader.c)
target_link_libraries(loader PRIVATE utils)
