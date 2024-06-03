project(example)

FILE(GLOB SOURCE_FILES
    src/*.cpp
)

FILE(GLOB HEADER_FILES
    src/*.hpp
)

# make variables visible to external CMakeLists.txt
set(SOURCE_FILES "${SOURCE_FILES}" CACHE INTERNAL "")
set(HEADER_FILES "${HEADER_FILES}" CACHE INTERNAL "")

add_library(examplelib SHARED ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(
    examplelib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)
