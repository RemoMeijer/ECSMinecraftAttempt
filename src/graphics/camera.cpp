#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "graphics/camera.h"
#include "world/world.h"
#include "physics/physicssystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Camera::Camera() {}

Camera::~Camera() {}

void Camera::mouse_callback(double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain the pitch to avoid flipping the camera
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Calculate the new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void Camera::processInput(GLFWwindow *window, float deltaTime) {
    // Close window on escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    velocity.x = 0.0f;
    velocity.z = 0.0f;

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (velocity.y == 0.0f) {
            velocity.y += 10.0f;
            onGround = false;
        }
    }

    // Camera movement
    float cameraSpeed = 10.0f;

    glm::vec3 front = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    glm::vec3 right = glm::normalize(glm::cross(front, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        velocity += front * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        velocity -= front * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        velocity -= right * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        velocity += right * cameraSpeed;
}

void Camera::updatePosition(World& world, float deltaTime) {
    velocity.y -= World::GRAVITY * deltaTime;

    PhysicsSystem::resolveCollision(world, boundingBox, cameraPos, velocity, onGround, deltaTime);
}