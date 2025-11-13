/**
 * Example: FPS Open World - With Vegetation
 * 
 * Demonstrates:
 * 1. Terrain with LOD
 * 2. Water rendering
 * 3. Vegetation (grass + trees)
 * 4. Wind animation
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
float cameraPitch = 30.0f;
float cameraYaw = 0.0f;

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                         "FPS Open World - Terrain + Water + Vegetation",
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

    // === WATER ===
    RenderLib::Water water;
    RenderLib::Water::Config waterConfig;
    waterConfig.width = 512.0f;
    waterConfig.height = 512.0f;
    waterConfig.position = glm::vec3(0, 20, 0);

    if (!water.init(waterConfig)) {
        std::cerr << "Failed to initialize water\n";
        return -1;
    }

    // === VEGETATION ===
    RenderLib::Vegetation vegetation;

    // Initialize grass
    RenderLib::Vegetation::GrassConfig grassConfig;
    grassConfig.grassCount = 2000;
    grassConfig.grassWidth = 0.5f;
    grassConfig.grassHeight = 1.5f;
    grassConfig.areaSize = {200.0f, 200.0f};

    auto grassTexture = std::make_shared<RenderLib::Texture2D>();
    // TODO: Load grass texture

    if (!vegetation.initGrass(grassConfig, grassTexture)) {
        std::cerr << "Failed to initialize grass\n";
        return -1;
    }

    // Initialize trees
    RenderLib::Vegetation::TreeConfig treeConfig;
    treeConfig.treeCount = 200;
    treeConfig.trunkRadius = 0.3f;
    treeConfig.treeHeight = 10.0f;
    treeConfig.areaSize = {300.0f, 300.0f};

    auto treeMesh = std::make_shared<RenderLib::Mesh>();
    auto treeBillboard = std::make_shared<RenderLib::Mesh>();
    // TODO: Load or generate tree models

    if (!vegetation.initTrees(treeConfig, treeMesh, treeBillboard)) {
        std::cerr << "Failed to initialize trees\n";
        return -1;
    }

    vegetation.setWind(0.5f, 1.0f);  // Strength and frequency

    // === MAIN LOOP ===
    double lastTime = glfwGetTime();
    float totalTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        totalTime += deltaTime;

        // Input
        const float moveSpeed = 50.0f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos += cameraFront * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraFront * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSpeed;
        }

        // Mouse look
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        static double lastMouseX = mouseX, lastMouseY = mouseY;
        double deltaX = mouseX - lastMouseX;
        double deltaY = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        cameraYaw += deltaX * 0.001f;
        cameraPitch -= deltaY * 0.001f;
        cameraPitch = glm::clamp(cameraPitch, -89.0f, 89.0f);

        glm::vec3 direction;
        direction.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        direction.y = sin(glm::radians(cameraPitch));
        direction.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront = glm::normalize(direction);

        // Gravity
        float terrainHeightAtCamera = terrain.getHeightAt(cameraPos.x, cameraPos.z);
        cameraPos.y = terrainHeightAtCamera + 1.8f;

        // Setup matrices
        glm::mat4 projection = glm::perspective(glm::radians(75.0f),
                                               (float)WINDOW_WIDTH / WINDOW_HEIGHT,
                                               0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        renderer.enableFrustumCulling(true);
        renderer.updateFrustum(projection * view);

        // === RENDER ===
        renderer.beginFrame();

        // Render terrain
        terrain.updateLOD(cameraPos);
        renderer.renderTerrain(&terrain, projection, view);

        // Render water
        water.updateWaves(deltaTime, 0.1f);
        renderer.renderWater(&water, projection, view);

        // Update and render vegetation
        vegetation.update(deltaTime);
        renderer.renderVegetation(&vegetation, projection, view);

        renderer.endFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
