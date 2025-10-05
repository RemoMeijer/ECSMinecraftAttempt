cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW)

project(ECSMinecraft LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

set(SOURCES
    lib/glad.c
    src/main.cpp
    src/graphics/camera.cpp
    src/graphics/shader.cpp
    src/world/chunksystem.cpp
    src/world/world.cpp
    src/physics/physicssystem.cpp
    src/world/raycast.cpp
)

add_executable(ecs-minecraft ${SOURCES})

target_include_directories(ecs-minecraft PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/lib/glad/include"
    "${CMAKE_SOURCE_DIR}/lib/glm"
    "${CMAKE_SOURCE_DIR}/lib"
    ${GLFW_INCLUDE_DIRS}
)

target_link_libraries(ecs-minecraft
    ${GLFW_LIBRARIES}
    OpenGL::GL
)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})