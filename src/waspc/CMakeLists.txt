
#add_subdirectory(diagnostic)

#adding waspc modules ##################################################################
add_subdirectory(utils)
add_subdirectory(objects)
add_subdirectory(memory)
add_subdirectory(decoder)
add_subdirectory(interpreter)
add_subdirectory(validation)
add_subdirectory(runtime)



# Merge all necesary libs
add_library(waspc INTERFACE)
target_link_libraries(waspc INTERFACE utils objects runtime)
