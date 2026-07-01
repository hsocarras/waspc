cmake_minimum_required(VERSION 3.20.0)

project(waspc VERSION 0.0.1 DESCRIPTION "Webassembly for process control")

# Enable testing for this project
include(CTest)

# Option for development build
option(WASPC_DEV_BUILD "Enable development build features" ON)

# Define the development flag if enabled
if(WASPC_DEV_BUILD)
    add_definitions(-DWASPC_CONFIG_DEV_FLAG=1)
endif()

# Add subdirectory for external projects' code
add_subdirectory(external)

#ADD main source code directory
add_subdirectory(src)