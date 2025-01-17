cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# Specify the path to your "include" folder (external libraries)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
set(SOURCE_FILES src/main.cpp)

# GLFW: Find and link GLFW
find_package(GLFW3 REQUIRED)

# GLAD: Add glad source files (assuming glad is in "include/glad")
add_library(glad src/glad.c)

# Add executable
add_executable(my_project ${SOURCE_FILES})

# Link libraries (GLFW, glad, etc.)
target_link_libraries(my_project glfw glad)

# Optional: Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)