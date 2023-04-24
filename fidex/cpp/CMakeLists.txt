cmake_minimum_required(VERSION 3.25.2)
project(fidex)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB FIDEX_SRC
    src/checkFun.cpp
    src/checkFun.h
    src/dataSet.cpp
    src/dataSet.h
    src/hyperbox.cpp
    src/hyperbox.h
    src/hyperspace.cpp
    src/hyperspace.h
)

FILE(GLOB HYPERLOCUS_SRC
    src/checkFun.cpp
    src/checkFun.h
    src/dataSet.cpp
    src/dataSet.h
)

# make variables visible to external CMakeLists.txt
set(FIDEX_SRC "${FIDEX_SRC}" CACHE INTERNAL "")
set(HYPERLOCUS_SRC "${HYPERLOCUS_SRC}" CACHE INTERNAL "")

# build executables
add_executable(fidex ${FIDEX_SRC} src/fidex.cpp)
add_executable(hyperLocus ${HYPERLOCUS_SRC} src/hyperLocus.cpp)

set_target_properties(
    fidex PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    hyperLocus PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
