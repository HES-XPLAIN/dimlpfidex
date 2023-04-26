cmake_minimum_required(VERSION 3.25.2)
project(fidexglo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB FIDEXGLO_SRC
    src/getRulesFun.cpp
    src/getRulesFun.h
)

FILE(GLOB FIDEXGLORULES_SRC
    src/checkFun.cpp
    src/checkFun.h
    src/dataSet.cpp
    src/dataSet.h
    src/fidexAlgo.cpp
    src/fidexAlgo.h
    src/hyperbox.cpp
    src/hyperbox.h
    src/hyperspace.cpp
    src/hyperspace.h
)

FILE(GLOB FIDEXGLOSTATS_SRC
    src/dataSet.cpp
    src/dataSet.h
    src/getRulesFun.cpp
    src/getRulesFun.h
)

# make variables visible to external CMakeLists.txt
set(FIDEXGLO_SRC "${FIDEXGLO_SRC}" CACHE INTERNAL "")
set(FIDEXGLORULES_SRC "${FIDEXGLORULES_SRC}" CACHE INTERNAL "")
set(FIDEXGLOSTATS_SRC "${FIDEXGLOSTATS_SRC}" CACHE INTERNAL "")

# build executables
add_executable(fidexGlo ${FIDEXGLO_SRC} src/fidexGlo.cpp)
add_executable(fidexGloRules ${FIDEXGLORULES_SRC} src/fidexGloRules.cpp)
add_executable(fidexGloStats ${FIDEXGLOSTATS_SRC} src/fidexGloStats.cpp)
add_executable(checkCovSize src/checkCovSize.cpp)

set_target_properties(
    fidexGlo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    fidexGloRules PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    fidexGloStats PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    checkCovSize PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
