cmake_minimum_required(VERSION 3.25.2)
project(dimlp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory("cpp")
include_directories("${CMAKE_SOURCE_DIR}/pybind")


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

pybind11_add_module(dimlp 
    ${COMMON_SRC}
    ${DIMLP_TRN_SRC}
    ${DIMLP_PRED_SRC}
    ${DIMLP_CLS_SRC}
    ${DIMLP_BT_SRC}
    ${DENS_CLS_SRC}
    ${DIMLP_RUL_SRC}
    ${CROSS_VALID_SRC}
    ${FCT_SRC}
    ${PYTHON_FILES}
)

set_target_properties(
    dimlp PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries(dimlp PUBLIC)