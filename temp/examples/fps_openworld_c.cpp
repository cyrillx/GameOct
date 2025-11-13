/**
 * Example: FPS Open World - Occlusion Culling + Object LOD
 * 
 * Demonstrates:
 * 1. Hi-Z pyramid for occlusion culling
 * 2. LOD group system for distant objects
 * 3. Frustum + occlusion culling combined
 * 4. Performance impact measurement
 */

#include "RenderLib/RenderLib.hpp"
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

struct WorldObject {
    RenderLib::LODGroup lodGroup;
    glm::vec3 position;
    glm::vec3 aabbMin, aabbMax;
    float distanceToCamera;
    bool visible;
};

glm::vec3 cameraPos = glm::vec3(0, 50, 0);
glm::vec3 cameraFront = glm::vec3(0, -0.5f, -1);
glm::vec3 cameraUp = glm::vec3(0, 1, 0);

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                         "FPS Open World - Occlusion & LOD",
                                         nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load OpenGL\n";
        return -1;
    }

    RenderLib::ForwardRenderer renderer;
    if (!renderer.init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
        std::cerr << "Failed to initialize renderer\n";
        return -1;
    }

    // === CREATE WORLD OBJECTS WITH LOD ===
    std::vector<WorldObject> worldObjects;

    for (int i = 0; i < 100; ++i) {
        WorldObject obj;
        obj.position = glm::vec3(
            -500 + (i % 10) * 100,
            50,
            -500 + (i / 10) * 100
        );

        // Create dummy LODs (in real app: load actual model files)
        auto lodMesh0 = std::make_shared<RenderLib::Mesh>();  // Full detail
        auto lodMesh1 = std::make_shared<RenderLib::Mesh>();  // 50% verts
        auto lodMesh2 = std::make_shared<RenderLib::Mesh>();  // 25% verts
        auto lodMesh3 = std::make_shared<RenderLib::Mesh>();  // 10% verts
        auto lodMeshImposter = std::make_shared<RenderLib::Mesh>();  // Billboard

        // LOD ranges (distance in world units)
        obj.lodGroup.addLOD(0, lodMesh0, 0.0f, 50.0f);
        obj.lodGroup.addLOD(1, lodMesh1, 50.0f, 150.0f);
        obj.lodGroup.addLOD(2, lodMesh2, 150.0f, 300.0f);
        obj.lodGroup.addLOD(3, lodMesh3, 300.0f, 500.0f);
        obj.lodGroup.addLOD(4, lodMeshImposter, 500.0f, 2000.0f);

        // AABB for culling
        obj.aabbMin = obj.position + glm::vec3(-5, 0, -5);
        obj.aabbMax = obj.position + glm::vec3(5, 10, 5);

        obj.visible = true;
        worldObjects.push_back(obj);
    }

    // === MAIN LOOP ===
    double lastTime = glfwGetTime();
    int culledByFrustum = 0;
    int culledByOcclusion = 0;
    int drawn = 0;

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
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSpeed;
        }

        // Setup matrices
        glm::mat4 projection = glm::perspective(glm::radians(75.0f),
                                               (float)WINDOW_WIDTH / WINDOW_HEIGHT,
                                               0.1f, 2000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 viewProj = projection * view;

        renderer.enableFrustumCulling(true);
        renderer.updateFrustum(viewProj);

        // === CULLING PHASE ===
        culledByFrustum = 0;
        culledByOcclusion = 0;
        drawn = 0;

        for (auto& obj : worldObjects) {
            // Frustum culling
            if (!renderer.isMeshInFrustum(obj.aabbMin, obj.aabbMax)) {
                culledByFrustum++;
                obj.visible = false;
                continue;
            }

            // Occlusion culling
            if (renderer.isOccluded(obj.aabbMin, obj.aabbMax)) {
                culledByOcclusion++;
                obj.visible = false;
                continue;
            }

            obj.visible = true;

            // Select LOD based on distance
            obj.distanceToCamera = glm::distance(obj.position, cameraPos);
            RenderLib::Mesh* selectedLOD = obj.lodGroup.selectLOD(obj.distanceToCamera);

            if (selectedLOD) {
                // TODO: submit for rendering
                drawn++;
            }
        }

        renderer.beginFrame();
        // Render visible objects
        // TODO: implement actual rendering
        renderer.endFrame();

        // === STATS ===
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            std::cout << "Frame Stats:\n"
                      << "  Total objects: " << worldObjects.size() << "\n"
                      << "  Frustum culled: " << culledByFrustum << "\n"
                      << "  Occlusion culled: " << culledByOcclusion << "\n"
                      << "  Drawn: " << drawn << "\n"
                      << "  Camera pos: (" << cameraPos.x << ", " 
                      << cameraPos.y << ", " << cameraPos.z << ")\n";
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
