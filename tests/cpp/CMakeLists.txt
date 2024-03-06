project(tests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3" )
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB TESTS_SRC
    headers/*.hpp
    src/*.cpp
)

# make variables visible to external CMakeLists.txt
set(TESTS_SRC "${TESTS_SRC}" CACHE INTERNAL "")

# build executables
add_executable(tests ${TESTS_SRC} tests.cpp)
add_dependencies(tests common)
add_dependencies(tests fidexLib)
add_dependencies(tests fidexGloLib)
add_dependencies(tests fidexGloRulesLib)
add_dependencies(tests fidexGloStatsLib)
target_link_libraries(tests common)
target_link_libraries(tests fidexLib)
target_link_libraries(tests fidexGloLib)
target_link_libraries(tests fidexGloRulesLib)
target_link_libraries(tests fidexGloStatsLib)

set_target_properties(
    tests PROPERTIES OUTPUT_NAME "tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
