

#add_library(samples STATIC samples.cc)
add_library(wasm_file_reader STATIC file_reader.cc)
set_property(TARGET wasm_file_reader PROPERTY CXX_STANDARD 17)