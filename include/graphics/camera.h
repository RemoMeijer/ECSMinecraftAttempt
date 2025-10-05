#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <iostream>

#include "physics/aabb.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class World;

class Camera {
    
public:
    Camera();
    ~Camera();

    GLFWwindow* window();

    glm::vec3 cameraPos   = glm::vec3(8.0f, 68.0f, 8.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    glm::vec3 velocity = glm::vec3(0.0f);
    AABB boundingBox{cameraPos, glm::vec3(0.8f, 4.0f, 0.8f)};

    float yaw = -90.0f;
    float pitch = 0.0f;

    float lastX = 600;
    float lastY = 300;
    bool firstMouse = true;

    void mouse_callback(double xpos, double ypos);
    void processInput(GLFWwindow *window, float deltaTime);
    void updatePosition(World& world, float deltaTime);

private:
    bool onGround = false;
};

#endif