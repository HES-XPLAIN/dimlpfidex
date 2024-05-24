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

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${DIMLPFIDEX_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating docs"
                   VERBATIM)

# add target so we can run the job easily
add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/_static)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/_templates)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND
                   ${SPHINX_EXECUTABLE} -b html
                   -Dbreathe_projects.dimlp=${DOXYGEN_OUTPUT_DIR}
                   ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                   # Other docs files to track should go here
                   ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                   ${DOXYGEN_INDEX_FILE}
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")

# add target so we can run the job easily
add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})
