

#adding loader module
#add_subdirectory(loader)

#adding runtime module
add_subdirectory(runtime)

add_library(waspc INTERFACE)
target_link_libraries(waspc INTERFACE runtime)
