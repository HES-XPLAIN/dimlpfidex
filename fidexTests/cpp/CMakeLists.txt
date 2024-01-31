project(fidexTests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3" )
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB FIDEXTESTS_SRC
    src/fidexGloTests.cpp
    headers/fidexGloTests.hpp
    src/fidexCommonTests.cpp
    headers/fidexCommonTests.hpp
    src/utils.cpp
    headers/utils.hpp
    headers/tests.hpp
)

# make variables visible to external CMakeLists.txt
set(FIDEXTESTS_SRC "${FIDEXTESTS_SRC}" CACHE INTERNAL "")

# build executables
add_executable(fidexTests ${FIDEXTESTS_SRC} tests.cpp)
add_dependencies(fidexTests fidexCommon)
# add_dependencies(fidexTests fidexGloRules)
target_link_libraries(fidexTests fidexCommon)
# target_link_libraries(fidexTestsProject fidexGloRules)

set_target_properties(
    fidexTests PROPERTIES OUTPUT_NAME "fidexTests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)