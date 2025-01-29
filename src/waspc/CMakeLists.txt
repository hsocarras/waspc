
#add_subdirectory(diagnostic)

#adding utils module
add_subdirectory(utils)



#adding loader module
add_subdirectory(objects)
add_subdirectory(validation)
#add_subdirectory(vm)
add_subdirectory(runtime)
# Merge all necesary libs
add_library(waspc INTERFACE)
target_link_libraries(waspc INTERFACE utils objects runtime)
