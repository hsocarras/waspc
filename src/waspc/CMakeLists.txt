
add_subdirectory(diagnostic)

#adding utils module
add_subdirectory(utils)

add_subdirectory(memory)

#adding loader module
add_subdirectory(loader)

add_subdirectory(runtime)


# Merge all necesary libs
add_library(waspc INTERFACE)
target_link_libraries(waspc INTERFACE diagnostic utils loader memory runtime)
