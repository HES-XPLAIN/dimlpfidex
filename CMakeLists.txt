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
endif()

# Enable AddressSanitizer
set(ENABLE_ASAN FALSE)

# create compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# add_subdirectory("example")
add_subdirectory("common")
add_subdirectory("dimlp")
add_subdirectory("fidex")
add_subdirectory("tests")
add_subdirectory("json")

# documentation
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/docs" ${CMAKE_MODULE_PATH})
add_subdirectory("docs")