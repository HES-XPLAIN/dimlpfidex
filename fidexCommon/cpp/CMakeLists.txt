project(fidexCommon)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB COMMON_SRC_FIDEX
    src/errorHandler.h
    src/dataSet.h
    src/dataSet.cpp
    src/checkFun.cpp
    src/checkFun.h
    src/hyperbox.cpp
    src/hyperbox.h
    src/hyperLocus.cpp
    src/hyperLocus.h
)

# make variables visible to external CMakeLists.txt
set(COMMON_SRC_FIDEX "${COMMON_SRC_FIDEX}" CACHE INTERNAL "")
add_library(fidexCommon STATIC ${COMMON_SRC_FIDEX})
