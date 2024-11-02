
#add_subdirectory(diagnostic)

#adding utils module
add_subdirectory(utils)

#add_subdirectory(memory)

#adding loader module
add_subdirectory(decoder)

add_subdirectory(object)

add_subdirectory(vm)

# Merge all necesary libs
add_library(waspc INTERFACE)
target_link_libraries(waspc INTERFACE utils decoder object vm)
