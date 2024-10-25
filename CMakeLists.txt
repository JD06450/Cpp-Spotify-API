cmake_minimum_required(VERSION 3.23.0)

set(CMAKE_CXX_STANDARD 20)

if (UNIX)
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
endif()

if (WIN32)
    set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")

    if (CMAKE_GENERATOR MATCHES "Ninja")
        set(CMAKE_MAKE_PROGRAM "C:/msys64/mingw64/bin/ninja.exe")
    endif()
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fconcepts-diagnostics-depth=5)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(Cpp-Spotify-API VERSION 0.1.0)

# TODO: Add option to compile to shared lib instead of static

add_subdirectory(src)

# add_subdirectory(tests)
