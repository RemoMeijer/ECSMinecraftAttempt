#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "shader.h"
#include "chunk.h"
#include "block.h"
#include "framebuffer.h"

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

    // Create the shader for the post-processing effect
    Shader screenShader("resources/screen.vert", "resources/screen.frag");

    // Create the framebuffer object for post-processing
    Framebuffer postProcessFBO(1200, 600); // Use your window dimensions

    // Create the chunk and build its mesh
    Chunk chunk;
    chunk.buildMesh();

    // --- SETUP FOR SCREEN QUAD (Pass 2) ---
    float quadVertices[] = { 
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    // **IMPORTANT FOR 3D:** Enable depth testing so faces render correctly

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        camera.processInput(window, deltaTime);

        // =====================================================================
        // === PASS 1: RENDER THE 3D SCENE TO OUR CUSTOM FRAMEBUFFER (FBO) ===
        // =====================================================================
        postProcessFBO.bind(); // Switch the render target to our FBO
        glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D
        // Clear the FBO's buffers
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate and configure the chunk shader
        chunkShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        
        chunkShader.setMat4("model", model);
        chunkShader.setMat4("view", view);
        chunkShader.setMat4("projection", projection);

        // Draw the chunk (this draws it into the FBO's texture)
        chunk.draw();
        
        // =====================================================================
        // === PASS 2: RENDER THE FBO TEXTURE TO THE SCREEN WITH AN EFFECT  ===
        // =====================================================================

        postProcessFBO.unbind();
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        
        // --- BIND BOTH TEXTURES to different texture units ---
        // Tell the shader that 'colorTexture' uniform is on texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcessFBO.getColorTexture());
        glUniform1i(glGetUniformLocation(screenShader.ID, "colorTexture"), 0);
        
        // Tell the shader that 'depthTexture' uniform is on texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, postProcessFBO.getDepthTexture());
        glUniform1i(glGetUniformLocation(screenShader.ID, "depthTexture"), 1);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- FINAL STEPS ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
