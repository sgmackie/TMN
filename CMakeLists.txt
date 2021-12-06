cmake_minimum_required(VERSION 3.8)

# Project info
project(TMNE VERSION 0.1.0 LANGUAGES C CXX)

# Set build flags
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_CXX_STANDARD 14)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /WL")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pedantic")
endif()

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DCORE_PLATFORM_WINDOWS=1")
endif()

# Export the flags for static analysis
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)

# Build each engine module
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/code/Core)

# Build the test projects
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/code/Raytracer)
