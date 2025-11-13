/**
 * Example: FPS Open World - Mirror Water with Reflections
 * 
 * Demonstrates:
 * 1. Terrain with LOD
 * 2. Mirror water with scene reflections
 * 3. Two-pass rendering (reflection + main)
 */

#include "RenderLib/RenderLib.hpp"
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

glm::vec3 cameraPos = glm::vec3(0, 50, 0);
glm::vec3 cameraFront = glm::vec3(0, -0.5f, -1);
glm::vec3 cameraUp = glm::vec3(0, 1, 0);

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                         "FPS Open World - Mirror Water",
                                         nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    RenderLib::ForwardRenderer renderer;
    if (!renderer.init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return -1;
    }

    // === TERRAIN ===
    RenderLib::Terrain terrain;
    RenderLib::Terrain::Config terrainConfig;
    terrainConfig.gridWidth = 256;
    terrainConfig.gridHeight = 256;
    terrainConfig.tileSize = 1.0f;
    terrainConfig.maxHeight = 100.0f;

    if (!terrain.generateFromNoise(terrainConfig)) {
        std::cerr << "Failed to generate terrain\n";
        return -1;
    }

    // === MIRROR WATER ===
    RenderLib::MirrorWater mirrorWater;
    RenderLib::MirrorWater::Config waterConfig;
    waterConfig.width = 512.0f;
    waterConfig.height = 512.0f;
    waterConfig.position = glm::vec3(0, 30, 0);
    waterConfig.reflectionTexSize = 512;  // Quality of reflection

    if (!mirrorWater.init(waterConfig)) {
        std::cerr << "Failed to initialize mirror water\n";
        return -1;
    }

    auto normalTexture = std::make_shared<RenderLib::Texture2D>();
    // TODO: Load normal map

    mirrorWater.setNormalTexture(normalTexture);

    // === MAIN LOOP ===
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        // Input
        const float moveSpeed = 50.0f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos += cameraFront * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraFront * moveSpeed;
        }

        // Gravity
        float terrainHeight = terrain.getHeightAt(cameraPos.x, cameraPos.z);
        cameraPos.y = terrainHeight + 1.8f;

        // Setup matrices
        glm::mat4 projection = glm::perspective(glm::radians(75.0f),
                                               (float)WINDOW_WIDTH / WINDOW_HEIGHT,
                                               0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // === TWO-PASS RENDERING ===

        // Pass 1: Render scene reflection (mirror pass)
        renderer.renderMirrorWaterReflection(&mirrorWater, projection, view);

        // Pass 2: Render main scene
        renderer.beginFrame();

        terrain.updateLOD(cameraPos);
        renderer.renderTerrain(&terrain, projection, view);

        mirrorWater.updateWaves(deltaTime, 0.1f);
        renderer.renderMirrorWater(&mirrorWater, projection, view);

        renderer.endFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
