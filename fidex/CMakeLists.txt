project(fidexProject)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-O3 -fopenmp")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fopenmp")
endif()

add_subdirectory("cpp")
include_directories("${CMAKE_SOURCE_DIR}/pybind")

file(GLOB PYTHON_FILES "pybind/*.cpp" "pybind/*.h")

# Build binding lib
pybind11_add_module(fidex
    ${FIDEX_COMMON_SRC}
    ${COMMON_SRC}
    ${FIDEX_SRC}
    ${FIDEXGLO_SRC}
    ${FIDEXGLORULES_SRC}
    ${FIDEXGLOSTATS_SRC}
    ${PYTHON_FILES}
)

set_target_properties(
    fidex PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dimlpfidex
)

target_link_libraries(fidex PUBLIC common)

install(TARGETS fidex LIBRARY DESTINATION ./dimlpfidex)
