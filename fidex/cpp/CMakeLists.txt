project(fidexProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB FIDEX_SRC
    src/fidexFct.cpp
    src/fidexFct.h
    ../../fidexGlo/cpp/src/hyperbox.cpp
    ../../fidexGlo/cpp/src//hyperbox.h
    ../../fidexGlo/cpp/src/hyperspace.cpp
    ../../fidexGlo/cpp/src//hyperspace.h
    ../../fidexGlo/cpp/src/fidexAlgo.cpp
    ../../fidexGlo/cpp/src/fidexAlgo.h
)

# make variables visible to external CMakeLists.txt
set(FIDEX_SRC "${FIDEX_SRC}" CACHE INTERNAL "")

# build executables
add_executable(fidexProject ${FIDEX_SRC} src/fidex.cpp)
add_dependencies(fidexProject common)
target_link_libraries(fidexProject common)

set_target_properties(
    fidexProject PROPERTIES OUTPUT_NAME "fidex" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
