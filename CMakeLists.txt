cmake_minimum_required(VERSION 3.21.2)
project(dimlpfidex)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# create compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_subdirectory("example")
add_subdirectory("common")
add_subdirectory("dimlp")
add_subdirectory("fidex")
add_subdirectory("fidexTests")
add_subdirectory("json")
