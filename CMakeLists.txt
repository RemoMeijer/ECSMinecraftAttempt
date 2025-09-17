cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW)

project(ECSMinecraft LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

add_executable(ecs-minecraft
    src/main.cpp
    src/glad.c
)


target_include_directories(ecs-minecraft PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    ${GLFW_INCLUDE_DIRS}

)

target_link_libraries(ecs-minecraft
    ${GLFW_LIBRARIES}
    OpenGL::GL
)
