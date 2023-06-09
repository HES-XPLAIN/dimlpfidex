project(fidexGloProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB FIDEXGLO_SRC
    src/fidexGloFct.cpp
    src/fidexGloFct.h
    src/getRulesFun.cpp
    src/getRulesFun.h
    src/dataSet.cpp
    src/dataSet.h
)

FILE(GLOB FIDEXGLORULES_SRC
    src/fidexGloRulesFct.cpp
    src/fidexGloRulesFct.h
    src/fidexAlgo.cpp
    src/fidexAlgo.h
    src/hyperspace.cpp
    src/hyperspace.h
)

FILE(GLOB FIDEXGLOSTATS_SRC
    src/fidexGloStatsFct.cpp
    src/fidexGloStatsFct.h
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
add_executable(fidexGloProject ${FIDEXGLO_SRC} src/fidexGlo.cpp)
add_executable(fidexGloRules ${FIDEXGLORULES_SRC} src/fidexGloRules.cpp)
add_executable(fidexGloStats ${FIDEXGLOSTATS_SRC} src/fidexGloStats.cpp)
add_executable(checkCovSize src/checkCovSize.cpp)
add_dependencies(fidexGloProject fidexCommon)
add_dependencies(fidexGloRules fidexCommon)
add_dependencies(fidexGloStats fidexCommon)
add_dependencies(checkCovSize fidexCommon)
target_link_libraries(fidexGloProject fidexCommon)
target_link_libraries(fidexGloRules fidexCommon)
target_link_libraries(fidexGloStats fidexCommon)
target_link_libraries(checkCovSize fidexCommon)

set_target_properties(
    fidexGloProject PROPERTIES OUTPUT_NAME "fidexGlo" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
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
