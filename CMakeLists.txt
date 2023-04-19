cmake_minimum_required(VERSION 3.25.2)
project(dimlpfidex)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB EXAMPLE_SRC
        dimlpfidex/example/bindings.cpp
        dimlpfidex/example/examples.cpp
        )

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
find_package(pybind11 CONFIG REQUIRED)
if (Python_FOUND AND Python_Interpreter_FOUND AND pybind11_FOUND)
    message(STATUS "Pybind11 directories: ${pybind11_DIR}")
    message(STATUS "Pybind11 include directories: ${pybind11_INCLUDE_DIRS}")
    include_directories(
        ${pybind11_INCLUDE_DIRS}
    )
endif()

pybind11_add_module(example ${EXAMPLE_SRC})

target_include_directories(example PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(example PRIVATE ${Python3_LIBRARIES})
