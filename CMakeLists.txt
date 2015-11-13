cmake_minimum_required(VERSION 3.3)
project(DrawToScreen)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_library(opengl_lib OpenGL)
find_library(glut_lib GLUT)
set(SOURCE_FILES main.cpp)
add_executable(DrawToScreen ${SOURCE_FILES})
target_link_libraries(DrawToScreen ${opengl_lib} ${glut_lib})
