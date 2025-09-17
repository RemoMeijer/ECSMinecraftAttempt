cmake_minimum_required(VERSION 3.10)
project(ECSMinecraft)

cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE GLVND)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

link_directories(${GLFW_LIBRARY_DIRS})

add_executable(ecs-minecraft src/main.cpp)
target_link_libraries(ecs-minecraft
    ${GLFW_LIBRARIES}
    OpenGL::GL
    GLEW
)
