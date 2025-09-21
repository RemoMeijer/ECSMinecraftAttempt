#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "graphics/camera.h"
#include "graphics/shader.h"
#include "world/world.h"

Camera camera;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 600, "Minecraft window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create a window!\n";
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, &camera);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD \n";
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    auto mouseCallbackWrapper = [](GLFWwindow* window, double xpos, double ypos) {
        Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        if (camera) {
            camera->mouse_callback(xpos, ypos);
        }
    };

    glfwSetCursorPosCallback(window, mouseCallbackWrapper);

    // Create the shader for our main 3D scene (the chunk)
    Shader chunkShader("resources/shader.vert", "resources/shader.frag");

    glEnable(GL_DEPTH_TEST);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load image with stb_image
    int width, height, nrChannels;
    unsigned char *data = stbi_load("resources/atlas.png", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // Free the image memory

    World world;
    world.createChunk(0, 0);
    world.update();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    int x = 0;
    int y = 0;
    while(!glfwWindowShouldClose(window)) {
        // Timing
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        camera.processInput(window, deltaTime);

        // Update        
        world.update();

        // Render
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        chunkShader.use();

         glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        chunkShader.setInt("texture_atlas", 0);

        // Set matrices that are the same for all chunks
        glm::mat4 view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        
        chunkShader.setMat4("view", view);
        chunkShader.setMat4("projection", projection);

        world.render(chunkShader);

        // --- FINAL STEPS ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
