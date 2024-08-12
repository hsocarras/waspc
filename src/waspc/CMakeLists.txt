
#adding utils module
add_subdirectory(utils)

#adding loader module
add_subdirectory(loader)


# Merge all necesary libs
add_library(waspc INTERFACE)
target_link_libraries(waspc INTERFACE loader)
