project(common)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB COMMON_SRC
    src/errorHandler.h
    src/stringI.cpp
    src/stringI.h
    src/dataSet.h
    src/dataSet.cpp
    src/checkFun.cpp
    src/checkFun.h
    src/hyperLocus.cpp
    src/hyperLocus.h
    src/antecedant.cpp
    src/antecedant.h
    src/rule.cpp
    src/rule.h
    src/parameters.cpp
    src/parameters.h
)

# make variables visible to external CMakeLists.txt
set(COMMON_SRC "${COMMON_SRC}" CACHE INTERNAL "")
add_library(common STATIC ${COMMON_SRC})
