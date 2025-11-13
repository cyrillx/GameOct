// gpu_cull_test.cpp
// Minimal test harness for GPUCuller using GLFW + GLEW

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "RenderLib/GPUCuller.hpp"
#include "RenderLib/Shader.hpp"

using namespace RenderLib;

static const int WIN_W = 1280;
static const int WIN_H = 720;

// Simple cube mesh
struct Mesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
};

Mesh createCube() {
    Mesh m;
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };
    unsigned int indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        0,4,7, 7,3,0,
        1,5,6, 6,2,1,
        3,2,6, 6,7,3,
        0,1,5, 5,4,0
    };
    m.indexCount = 36;

    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &m.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
    return m;
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "GPU Culler Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to init GLEW\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Create mesh
    Mesh cube = createCube();

    // Create many instance matrices and an instance VBO
    const int INSTANCE_COUNT = 4096; // many instances
    std::vector<glm::mat4> instances(INSTANCE_COUNT);
    int side = (int)ceil(pow((float)INSTANCE_COUNT, 1.0f/3.0f));
    int idx = 0;
    for (int z = 0; z < side && idx < INSTANCE_COUNT; ++z) {
        for (int y = 0; y < side && idx < INSTANCE_COUNT; ++y) {
            for (int x = 0; x < side && idx < INSTANCE_COUNT; ++x) {
                glm::vec3 pos = glm::vec3((x - side/2) * 2.5f, (y - side/2) * 2.5f, (z - side/2) * 2.5f);
                instances[idx++] = glm::translate(glm::mat4(1.0f), pos);
            }
        }
    }

    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * INSTANCE_COUNT, instances.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create a renderVAO for culling: a single point with instance attributes bound (mat4)
    GLuint renderVAO;
    glGenVertexArrays(1, &renderVAO);
    glBindVertexArray(renderVAO);
    // one dummy point
    GLuint pointVBO;
    glGenBuffers(1, &pointVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    float pt[3] = {0.0f, 0.0f, 0.0f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(pt), pt, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Bind instanceVBO into locations 4..7 for the cull shader
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    for (int i = 0; i < 4; ++i) {
        GLuint loc = 4 + i;
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * 4 * i));
        glVertexAttribDivisor(loc, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create GPUCuller
    GPUCuller culler;
    if (!culler.init(INSTANCE_COUNT)) {
        std::cerr << "GPUCuller init failed\n";
        return -1;
    }
    culler.setInstanceBuffer(instanceVBO, INSTANCE_COUNT);

    // Create shader for final instanced mesh rendering
    auto meshShader = std::make_shared<Shader>("shaders/instanced_mesh.vert", "shaders/instanced_mesh.frag");
    if (!meshShader) {
        std::cerr << "Failed to load mesh shader\n";
        return -1;
    }

    // Simple camera
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), WIN_W / (float)WIN_H, 0.1f, 1000.0f);
    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 80.0f);
    glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Build viewProj for culling
        glm::mat4 viewProj = proj * view;
        // Perform culling: renderVAO contains instance matrices
        culler.cull(renderVAO, viewProj);

        // Render visible instances using mesh shader; mesh shader expects instance matrix at locations 4..7
        meshShader->use();
        meshShader->setMat4("uViewProj", viewProj);

        // Draw instanced using results of culling
        culler.drawInstanced(cube.vao, cube.indexCount);

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &pointVBO);
    glDeleteVertexArrays(1, &renderVAO);
    glDeleteVertexArrays(1, &cube.vao);
    glDeleteBuffers(1, &cube.vbo);
    glDeleteBuffers(1, &cube.ebo);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
