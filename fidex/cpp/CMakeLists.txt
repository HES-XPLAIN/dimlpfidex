project(fidexProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3 -Wunused" )
# Warning, these flags slows down the execution
# set(CMAKE_CXX_FLAGS "-O3 -fsanitize=address -fsanitize=leak -fsanitize=undefined")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB FIDEX_SRC
    src/fidexFct.cpp
    src/fidexFct.h
    src/hyperLocus.cpp
    src/hyperLocus.h
    src/hyperbox.cpp
    src//hyperbox.h
    src/hyperspace.cpp
    src//hyperspace.h
    src/fidexAlgo.cpp
    src/fidexAlgo.h
)

FILE(GLOB FIDEXGLO_SRC
    src/fidexGloFct.cpp
    src/fidexGloFct.h
    src/getRulesFun.cpp
    src/getRulesFun.h
    src/fidexAlgo.cpp
    src/fidexAlgo.h
    src/hyperLocus.cpp
    src/hyperLocus.h
    src/hyperbox.cpp
    src/hyperbox.h
    src/hyperspace.cpp
    src/hyperspace.h
)

FILE(GLOB FIDEXGLORULES_SRC
    src/fidexGloRulesFct.cpp
    src/fidexGloRulesFct.h
    src/fidexAlgo.cpp
    src/fidexAlgo.h
    src/hyperLocus.cpp
    src/hyperLocus.h
    src/hyperbox.cpp
    src/hyperbox.h
    src/hyperspace.cpp
    src/hyperspace.h
)

FILE(GLOB FIDEXGLOSTATS_SRC
    src/fidexGloStatsFct.cpp
    src/fidexGloStatsFct.h
    src/getRulesFun.cpp
    src/getRulesFun.h
)

# make variables visible to external CMakeLists.txt
set(FIDEX_SRC "${FIDEX_SRC}" CACHE INTERNAL "")
set(FIDEXGLO_SRC "${FIDEXGLO_SRC}" CACHE INTERNAL "")
set(FIDEXGLORULES_SRC "${FIDEXGLORULES_SRC}" CACHE INTERNAL "")
set(FIDEXGLOSTATS_SRC "${FIDEXGLOSTATS_SRC}" CACHE INTERNAL "")
set(FIDEXGLOTESTS_SRC "${FIDEXGLOTESTS_SRC}" CACHE INTERNAL "")
add_library(fidexGloRulesLib STATIC ${FIDEXGLORULES_SRC})

# build executables
add_executable(fidexExe ${FIDEX_SRC} src/fidex.cpp)
add_executable(fidexGlo ${FIDEXGLO_SRC} src/fidexGlo.cpp)
add_executable(fidexGloRules ${FIDEXGLORULES_SRC} src/fidexGloRules.cpp)
add_executable(fidexGloStats ${FIDEXGLOSTATS_SRC} src/fidexGloStats.cpp)
add_dependencies(fidexExe common)
add_dependencies(fidexGlo common)
add_dependencies(fidexGloRulesLib common)
add_dependencies(fidexGloRules common)
add_dependencies(fidexGloStats common)
target_link_libraries(fidexExe common)
target_link_libraries(fidexGlo common)
target_link_libraries(fidexGloRulesLib common)
target_link_libraries(fidexGloRules common)
target_link_libraries(fidexGloStats common)

set_target_properties(
    fidexExe PROPERTIES OUTPUT_NAME "fidex" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    fidexGlo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    fidexGloRules PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    fidexGloStats PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
