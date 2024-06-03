project(example)

add_subdirectory("cpp")
include_directories("${CMAKE_SOURCE_DIR}/cpp/include")
include_directories("${CMAKE_SOURCE_DIR}/pybind")

file(GLOB PYTHON_FILES "pybind/*.cpp" "pybind/*.hpp")

# Build binding lib
pybind11_add_module(example
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${PYTHON_FILES}
)

set_target_properties(
    example PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dimlpfidex
)

target_link_libraries(example PUBLIC)
