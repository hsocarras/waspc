
#add_subdirectory(diagnostic)

#adding waspc modules ##################################################################
add_subdirectory(interpreter)
add_subdirectory(objects)
add_subdirectory(runtime)
add_subdirectory(utils)
add_subdirectory(validation)


# Merge all necesary libs
add_library(waspc INTERFACE)
target_link_libraries(waspc INTERFACE utils objects runtime)
