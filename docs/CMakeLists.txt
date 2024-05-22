find_package(Doxygen REQUIRED)

# Find all the public headers
get_target_property(COMMON_PUBLIC_HEADER_DIR common INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(DIMLP_PUBLIC_HEADER_DIR dimlp INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(FIDEX_PUBLIC_HEADER_DIR fidex INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE DIMLPFIDEX_PUBLIC_HEADERS
    ${COMMON_PUBLIC_HEADER_DIR}/*.h
    ${DIMLP_PUBLIC_HEADER_DIR}/*.h
    ${FIDEX_PUBLIC_HEADER_DIR}/*.h
)

set(DOXYGEN_DIRS
    ${PROJECT_SOURCE_DIR}/common/cpp/src
    ${PROJECT_SOURCE_DIR}/dimlp/cpp/src
    ${PROJECT_SOURCE_DIR}/fidex/cpp/src
)
string(REPLACE ";" " " DOXYGEN_DIRS_STR "${DOXYGEN_DIRS}")
set(DOXYGEN_INPUT_DIR ${DOXYGEN_DIRS_STR})

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${DIMLPFIDEX_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating docs")

add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
