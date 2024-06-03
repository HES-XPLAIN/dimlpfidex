project(dimlp)

add_subdirectory("cpp")
include_directories("${CMAKE_SOURCE_DIR}/pybind")

file(GLOB PYTHON_FILES "pybind/*.cpp" "pybind/*.h")

# Check Python
find_package(Python COMPONENTS Interpreter Development REQUIRED)
if(Python_FOUND)
    message(STATUS "Found Python ${Python_VERSION} ${Python_EXECUTABLE}")
    message(STATUS "Found Python libs: ${Python_LIBRARIES}")
    message(STATUS "Python include directories: ${Python_INCLUDE_DIRS}")
    message(STATUS "Python library directories: ${Python_LIBRARY_DIRS}")
    include_directories(
        ${Python_INCLUDE_DIRS}
    )
else()
    message(STATUS "Python NOT found!")
endif()

# Check pybind11
# find_package(pybind11 REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
if (Python_FOUND AND Python_Interpreter_FOUND AND pybind11_FOUND)
    message(STATUS "Pybind11 directories: ${pybind11_DIR}")
    message(STATUS "Pybind11 include directories: ${pybind11_INCLUDE_DIRS}")
    include_directories(
        ${pybind11_INCLUDE_DIRS}
    )
endif()

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
