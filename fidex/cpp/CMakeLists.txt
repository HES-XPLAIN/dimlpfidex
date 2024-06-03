project(fidexProject)

FILE (GLOB FIDEX_COMMON_SRC
    src/hyperLocus.cpp
    src/hyperLocus.h
    src/fidexAlgo.cpp
    src/fidexAlgo.h
    src/hyperbox.cpp
    src/hyperbox.h
    src/hyperspace.cpp
    src/hyperspace.h
)

FILE(GLOB FIDEX_SRC
    src/fidexFct.cpp
    src/fidexFct.h
)

FILE(GLOB FIDEXGLO_SRC
    src/fidexGloFct.cpp
    src/fidexGloFct.h
)

FILE(GLOB FIDEXGLORULES_SRC
    src/fidexGloRulesFct.cpp
    src/fidexGloRulesFct.h
)

FILE(GLOB FIDEXGLOSTATS_SRC
    src/fidexGloStatsFct.cpp
    src/fidexGloStatsFct.h
)

# make variables visible to external CMakeLists.txt
set(FIDEX_COMMON_SRC "${FIDEX_COMMON_SRC}" CACHE INTERNAL "")
set(FIDEX_SRC "${FIDEX_SRC}" CACHE INTERNAL "")
set(FIDEXGLO_SRC "${FIDEXGLO_SRC}" CACHE INTERNAL "")
set(FIDEXGLORULES_SRC "${FIDEXGLORULES_SRC}" CACHE INTERNAL "")
set(FIDEXGLOSTATS_SRC "${FIDEXGLOSTATS_SRC}" CACHE INTERNAL "")
set(FIDEXGLOTESTS_SRC "${FIDEXGLOTESTS_SRC}" CACHE INTERNAL "")

add_library(fidexLib STATIC ${FIDEX_COMMON_SRC} ${FIDEX_SRC})
add_library(fidexGloLib STATIC ${FIDEX_COMMON_SRC} ${FIDEXGLO_SRC})
add_library(fidexGloRulesLib STATIC ${FIDEX_COMMON_SRC} ${FIDEXGLORULES_SRC})
add_library(fidexGloStatsLib STATIC ${FIDEX_COMMON_SRC} ${FIDEXGLOSTATS_SRC})


# build executables
add_executable(fidexExe ${FIDEX_COMMON_SRC} ${FIDEX_SRC} src/fidex.cpp)
add_executable(fidexGlo ${FIDEX_COMMON_SRC} ${FIDEXGLO_SRC} src/fidexGlo.cpp)
add_executable(fidexGloRules ${FIDEX_COMMON_SRC} ${FIDEXGLORULES_SRC} src/fidexGloRules.cpp)
add_executable(fidexGloStats ${FIDEX_COMMON_SRC} ${FIDEXGLOSTATS_SRC} src/fidexGloStats.cpp)
add_dependencies(fidexExe common)
add_dependencies(fidexGlo common)
add_dependencies(fidexGloRulesLib common)
add_dependencies(fidexGloRules common)
add_dependencies(fidexGloStats common)
target_link_libraries(fidexExe common)
target_link_libraries(fidexGlo common)
target_link_libraries(fidexGloRules common)
target_link_libraries(fidexGloStats common)

target_link_libraries(fidexLib common)
target_link_libraries(fidexGloLib common)
target_link_libraries(fidexGloRulesLib common)
target_link_libraries(fidexGloStatsLib common)

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
