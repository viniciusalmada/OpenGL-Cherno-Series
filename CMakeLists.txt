cmake_minimum_required(VERSION 3.15)
project(TutoCherno)

set(CMAKE_CXX_STANDARD 11)

set(neededLibs GLEW glfw GL)

add_compile_definitions(GLEW_STATIC)
add_compile_definitions(GLM_FORCE_CXX11)

add_executable(TutoCherno Application.cpp #[[Cube.cpp Cube.h Renderer.cpp Renderer.h]])

target_link_libraries(TutoCherno ${neededLibs})