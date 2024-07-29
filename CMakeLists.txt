cmake_minimum_required(VERSION 3.20.0)

project(waspc VERSION 0.0.1 DESCRIPTION "Webassembly for process control")

# Enable testing for this project
#include(CTest)

# Add subdirectory for external projects' code
#add_subdirectory(external)

#ADD main source code directory
add_subdirectory(src)