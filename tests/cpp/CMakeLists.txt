project(tests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-O3")
endif()
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
add_dependencies(tests dimlpTrnLib)
add_dependencies(tests dimlpPredLib)
add_dependencies(tests dimlpClsLib)
add_dependencies(tests dimlpBTLib)
add_dependencies(tests densClsLib)
add_dependencies(tests dimlpRulLib)
target_link_libraries(tests common)
target_link_libraries(tests fidexLib)
target_link_libraries(tests fidexGloLib)
target_link_libraries(tests fidexGloRulesLib)
target_link_libraries(tests fidexGloStatsLib)
target_link_libraries(tests dimlpTrnLib)
target_link_libraries(tests dimlpPredLib)
target_link_libraries(tests dimlpClsLib)
target_link_libraries(tests dimlpBTLib)
target_link_libraries(tests densClsLib)
target_link_libraries(tests dimlpRulLib)

set_target_properties(
    tests PROPERTIES OUTPUT_NAME "tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
