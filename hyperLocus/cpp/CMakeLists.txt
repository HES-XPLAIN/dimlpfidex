project(hyperlocus)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB HYPERLOCUS_SRC
    src/hyperLocusFct.cpp
    src/hyperLocusFct.h
    src/checkFun.cpp
    src/checkFun.h
    src/dataSet.cpp
    src/dataSet.h
)

# make variables visible to external CMakeLists.txt
set(HYPERLOCUS_SRC "${HYPERLOCUS_SRC}" CACHE INTERNAL "")

# build executables
add_executable(hyperLocus ${HYPERLOCUS_SRC} ${COMMON_SRC_FIDEX} src/hyperLocus.cpp)

set_target_properties(
    hyperLocus PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
