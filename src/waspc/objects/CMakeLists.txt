



add_library(objects STATIC  error.c export.c function.c global.c memory.c module_state.c  module_instance.c result.c was_deftype.c)
#target_link_libraries(objects PRIVATE diagnostic utils)
