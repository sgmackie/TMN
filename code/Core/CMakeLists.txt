cmake_minimum_required(VERSION 3.8)

# Project info
project(Core VERSION 0.1.0 LANGUAGES C CXX)

# Add source files for module library
set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
add_library(${PROJECT_NAME} 
    Source/PlatformWindows.cpp
    Source/Unicode.cpp
    Source/AllocatorVirtualMemory.cpp
)

# Add include path
target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/Include
)

# Add platform libraries
if(MSVC)
    find_library(WIN_KERNEL32_LIB kernel32 NAMES kernel32.lib kernel32.dll)
    message(STATUS "Windows: Kernel32 Loaded @ ${WIN_KERNEL32_LIB}")
    target_link_libraries(${PROJECT_NAME} 
        ${WIN_KERNEL32_LIB}
    )
else()
endif()
