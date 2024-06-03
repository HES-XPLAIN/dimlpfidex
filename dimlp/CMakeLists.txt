project(dimlp)

add_subdirectory("cpp")
include_directories("${CMAKE_SOURCE_DIR}/pybind")

file(GLOB PYTHON_FILES "pybind/*.cpp" "pybind/*.h")

# Build binding lib
pybind11_add_module(dimlp
    ${DIMLP_COMMON_SRC}
    ${COMMON_SRC}
    ${DIMLP_TRN_SRC}
    ${DIMLP_PRED_SRC}
    ${DIMLP_CLS_SRC}
    ${DIMLP_BT_SRC}
    ${DENS_CLS_SRC}
    ${DIMLP_RUL_SRC}
    ${CROSS_VALID_SRC}
    ${PYTHON_FILES}
)

set_target_properties(
    dimlp PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dimlpfidex
)

target_link_libraries(dimlp PUBLIC common)

install(TARGETS dimlp LIBRARY DESTINATION ./dimlpfidex)
