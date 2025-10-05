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
#include "world/raycast.h"

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
    Shader chunkShader("res/shaders/shader.vert", "res/shaders/shader.frag");
    // Create the shader for the wireframe
    Shader wireframeShader("res/shaders/wireframe.vert", "res/shaders/wireframe.frag");


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

    float wireframeVertices[] = {
        // positions
        0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        //
        0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
        //
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 1.0f
    };

    unsigned int wireframeVAO, wireframeVBO;
    glGenVertexArrays(1, &wireframeVAO);
    glGenBuffers(1, &wireframeVBO);

    glBindVertexArray(wireframeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wireframeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wireframeVertices), wireframeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
        camera.processInput(window, world, deltaTime);
        camera.updatePosition(world, deltaTime);

        // Update
        world.updateChunksAroundPlayer(camera.cameraPos);        
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
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
        
        chunkShader.setMat4("view", view);
        chunkShader.setMat4("projection", projection);

        world.render(chunkShader);

        auto hit = RaycastSystem::cast(world, camera.cameraPos, camera.cameraFront, 5.0f);
        if (hit.has_value()) {
            wireframeShader.use();

            // Create a model matrix to move the wireframe to the hit block's position
            glm::mat4 model = glm::mat4(1.0f);
            // We need to translate to the block's integer coordinates
            model = glm::translate(model, glm::vec3(hit->blockPosition));
            // 2. Translate to the center of the block to scale from the center
            model = glm::translate(model, glm::vec3(0.5f));
            // 3. Scale the cube up by a tiny amount
            model = glm::scale(model, glm::vec3(1.002f));
            // 4. Translate back
            model = glm::translate(model, glm::vec3(-0.5f));

            // Set all the matrices for the wireframe shader
            wireframeShader.setMat4("model", model);
            wireframeShader.setMat4("view", view); // Use the same view matrix as the world
            wireframeShader.setMat4("projection", projection); // Use the same projection matrix

            glLineWidth(3.0f);

            // Draw the wireframe cube
            glBindVertexArray(wireframeVAO);
            glDrawArrays(GL_LINES, 0, 24); // 24 vertices for 12 lines
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
