project(tests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3 -fopenmp")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fopenmp")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable static compilation for MingW
# see https://cython.readthedocs.io/en/latest/src/tutorial/appendix.html
if (WIN32)
    if(MINGW)
        set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} -static-libgcc -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
        set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
        set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -liconv -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
        set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()
endif()

add_subdirectory("cpp")

set_target_properties(
    tests PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dimlpfidex
)

target_link_libraries(tests PUBLIC)
