cmake_minimum_required(VERSION 3.21.2)
project(dimlpfidex)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-O3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    # Warning, these flags slows down the execution
    # set(CMAKE_CXX_FLAGS "-O3 -fsanitize=address -fsanitize=leak -fsanitize=undefined")
endif()

# Enable AddressSanitizer
set(ENABLE_ASAN FALSE)

# create compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set build options
option(BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
message(STATUS "BUILD_DOCUMENTATION is set to ${BUILD_DOCUMENTATION}")

if(MSVC)
    message(STATUS "MSVC version: ${MSVC_VERSION}")

    if(ENABLE_ASAN)
        # Dont know the exact version but its somewhere there
        if(MSVC_VERSION LESS 1925)
            message(FATAL_ERROR "AddressSanitizer enabled but compiler doesn't support it - cannot continue.")
        endif()

        message(STATUS "Enabling AddressSanitizer for this configuration")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
    endif()

    if(FORCE_STATIC_VCRT)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc /O2")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHa /W3")

        set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
        )

        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        endforeach()

        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.lib /NODEFAULTLIB:MSVCRTD.lib")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.lib /NODEFAULTLIB:MSVCRTD.lib")
    endif()

    add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Do not show CRT warnings
endif(MSVC)

# Check Python
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
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

# add_subdirectory("example")
add_subdirectory("common")
add_subdirectory("dimlp")
add_subdirectory("fidex")
add_subdirectory("tests")
add_subdirectory("json")

# documentation
if (BUILD_DOCUMENTATION)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/docs" ${CMAKE_MODULE_PATH})
    add_subdirectory("docs")
endif()
