/**
 * Example: FPS Open World - Terrain and Water
 * 
 * Demonstrates:
 * 1. Procedurally generated terrain with LOD
 * 2. Water rendering with reflections and waves
 * 3. Height-based fog
 * 4. Camera movement on terrain
 */

#include "RenderLib/RenderLib.hpp"
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Window size
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

// Camera
glm::vec3 cameraPos = glm::vec3(0, 50, 0);
glm::vec3 cameraFront = glm::vec3(0, -0.5f, -1);
glm::vec3 cameraUp = glm::vec3(0, 1, 0);
float cameraPitch = 30.0f;
float cameraYaw = 0.0f;

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                         "FPS Open World - Terrain & Water",
                                         nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Vsync

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load OpenGL\n";
        return -1;
    }

    // Initialize renderer
    RenderLib::ForwardRenderer renderer;
    if (!renderer.init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
        std::cerr << "Failed to initialize renderer\n";
        return -1;
    }

    // === CREATE TERRAIN ===
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

    // Create terrain textures
    auto diffuseTex = std::make_shared<RenderLib::Texture2D>();
    auto normalTex = std::make_shared<RenderLib::Texture2D>();
    // TODO: Load actual textures from files
    // diffuseTex->fromMemory(...);
    // normalTex->fromMemory(...);
    
    terrain.setDiffuseTexture(diffuseTex);
    terrain.setNormalTexture(normalTex);

    // === CREATE WATER ===
    RenderLib::Water water;
    RenderLib::Water::Config waterConfig;
    waterConfig.width = 512.0f;
    waterConfig.height = 512.0f;
    waterConfig.position = glm::vec3(0, 20, 0);
    waterConfig.shallowColor = glm::vec3(0.1f, 0.6f, 0.8f);
    waterConfig.deepColor = glm::vec3(0.0f, 0.1f, 0.3f);

    if (!water.init(waterConfig)) {
        std::cerr << "Failed to initialize water\n";
        return -1;
    }

    auto normalMapTex = std::make_shared<RenderLib::Texture2D>();
    // TODO: Load normal map texture

    // === MAIN LOOP ===
    double lastTime = glfwGetTime();
    float totalTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        totalTime += deltaTime;

        // Input handling
        const float moveSpeed = 50.0f * deltaTime;
        const float mouseSensitivity = 0.001f;

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
        double deltaX = (mouseX - lastMouseX);
        double deltaY = (mouseY - lastMouseY);
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        cameraYaw += deltaX * mouseSensitivity;
        cameraPitch -= deltaY * mouseSensitivity;
        cameraPitch = glm::clamp(cameraPitch, -89.0f, 89.0f);

        // Update camera direction from pitch/yaw
        glm::vec3 direction;
        direction.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        direction.y = sin(glm::radians(cameraPitch));
        direction.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront = glm::normalize(direction);

        // Simple gravity (player height above terrain)
        float terrainHeightAtCamera = terrain.getHeightAt(cameraPos.x, cameraPos.z);
        cameraPos.y = terrainHeightAtCamera + 1.8f;  // 1.8 = eye height

        // Setup matrices
        glm::mat4 projection = glm::perspective(glm::radians(75.0f),
                                               (float)WINDOW_WIDTH / WINDOW_HEIGHT,
                                               0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 viewProj = projection * view;

        // Update culling
        renderer.enableFrustumCulling(true);
        renderer.updateFrustum(viewProj);

        // === RENDER ===
        renderer.beginFrame();

        // Render terrain
        terrain.updateLOD(cameraPos);
        renderer.renderTerrain(&terrain, projection, view);

        // Update water waves
        water.updateWaves(deltaTime, 0.1f);

        // Render water
        renderer.renderWater(&water, projection, view);

        renderer.endFrame();

        // Copy HDR buffer to backbuffer with tone mapping
        // TODO: implement post-processing

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
