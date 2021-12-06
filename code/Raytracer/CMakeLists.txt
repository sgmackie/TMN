cmake_minimum_required(VERSION 3.8)

# Project info
project(Raytracer VERSION 0.1.0 LANGUAGES C CXX)

# List source files
add_executable(${PROJECT_NAME} 
    Source/main.cpp
)

# Add library module dependancies
add_dependencies(${PROJECT_NAME} 
    Core
)

# Add library include paths
target_include_directories(${PROJECT_NAME} 
    PUBLIC ${CMAKE_SOURCE_DIR}/code/Core/Include
)

# Link library modules
set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
target_link_libraries(${PROJECT_NAME} 
    Core
)
