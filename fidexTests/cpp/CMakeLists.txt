project(fidexTests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3" )
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB FIDEXTESTS_SRC
    headers/*.hpp
    src/*.cpp
)

# make variables visible to external CMakeLists.txt
set(FIDEXTESTS_SRC "${FIDEXTESTS_SRC}" CACHE INTERNAL "")

# build executables
add_executable(fidexTests ${FIDEXTESTS_SRC} fidexTests.cpp)
add_dependencies(fidexTests fidexCommon)
add_dependencies(fidexTests fidexGloRulesLib)
target_link_libraries(fidexTests fidexCommon)
target_link_libraries(fidexTests fidexGloRulesLib)

set_target_properties(
    fidexTests PROPERTIES OUTPUT_NAME "fidexTests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)