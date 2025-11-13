// full_feature_demo.cpp
// Demonstrates usage of all implemented rendering techniques in the workspace.
// This is a high-level example: fill in resource paths and engine initialisation
// details to match your project (windowing, input, asset loading).

#include "RenderLib/Renderer.hpp"
#include "RenderLib/ForwardRenderer.hpp"
#include "RenderLib/PostProcessingPipeline.hpp"
#include "RenderLib/ShadowManager.hpp"
#include "RenderLib/LightDirectional.hpp"
#include "RenderLib/LightPoint.hpp"
#include "RenderLib/LightSpot.hpp"
#include "RenderLib/Terrain.hpp"
#include "RenderLib/Water.hpp"
#include "RenderLib/Vegetation.hpp"
#include "RenderLib/Camera.hpp"
#include "RenderLib/Scene.hpp"

#include <iostream>
#include <memory>
#include <chrono>

using namespace RenderLib;

int main(int argc, char** argv) {
    // --- Initialize platform/windowing ---
    // TODO: Replace with your actual window/GL context creation code
    // e.g., SDL/GLFW init, glewInit(), set viewport etc.
    if (!initializePlatform()) {
        std::cerr << "Platform initialization failed\n";
        return -1;
    }

    int width = 1920;
    int height = 1080;

    // --- Create core renderer and systems ---
    ForwardRenderer forwardRenderer;
    if (!forwardRenderer.init(width, height)) {
        std::cerr << "ForwardRenderer init failed\n";
        return -1;
    }

    // Shadow manager (RSM + shadow atlas are implemented in repository)
    ShadowManager shadowManager;
    shadowManager.init(width, height);

    // Post-processing pipeline (SSR, Bloom, Atmosphere, SunEffects, ColorGrading, EdgeDetection)
    PostProcessingPipeline pp;
    if (!pp.init(width, height)) {
        std::cerr << "PostProcessingPipeline init failed\n";
        return -1;
    }

    // --- Scene setup ---
    Scene scene;

    // Camera
    Camera camera(glm::vec3(0.0f, 2.0f, 6.0f), 75.0f);
    camera.setAspectRatio(width / (float)height);

    // Lights
    auto sun = std::make_shared<LightDirectional>();
    sun->setDirection(glm::normalize(glm::vec3(-0.3f, -1.0f, -0.5f)));
    sun->setColor(glm::vec3(1.0f, 0.95f, 0.9f));
    sun->setIntensity(2.0f);
    sun->enableRSM(true);
    sun->setRSMQuality(2048);
    scene.addLight(sun);

    // Point light (example: torch)
    auto torch = std::make_shared<LightPoint>();
    torch->setPosition(glm::vec3(2.0f, 1.5f, 0.0f));
    torch->setColor(glm::vec3(1.0f, 0.6f, 0.3f));
    torch->setRadius(8.0f);
    torch->enableRSM(true);
    scene.addLight(torch);

    // Spot light (flashlight)
    auto flash = std::make_shared<LightSpot>();
    flash->setPosition(glm::vec3(0.0f, 1.6f, 0.0f));
    flash->setDirection(glm::vec3(0.0f, 0.0f, -1.0f));
    flash->setInnerAngle(glm::radians(12.0f));
    flash->setOuterAngle(glm::radians(16.0f));
    flash->setColor(glm::vec3(1.0f));
    flash->setIntensity(3.0f);
    flash->enableRSM(true);
    scene.addLight(flash);

    // Terrain
    Terrain terrain;
    terrain.generateFromNoise({1024, 1024, 0.5f, 200.0f});
    terrain.setDiffuseTexture(loadTexture("textures/terrain_diffuse.png"));
    scene.setTerrain(&terrain);

    // Water
    Water water;
    water.init(512, 512);
    water.setHeight(0.2f);
    water.setFresnelParams(0.02f, 1.33f);
    water.setReflectionTexture(shadowManager.getShadowAtlas()); // example usage
    scene.setWater(&water);

    // Vegetation (instanced grass/trees)
    Vegetation vegetation;
    vegetation.init(10000); // spawn count
    vegetation.setWindSystemEnabled(true);
    scene.setVegetation(&vegetation);

    // Load models/meshes
    // TODO: Add calls to load and place meshes (houses, rocks, props)
    // e.g. scene.addMesh(loadMesh("models/house.obj"), transform);

    // Configure Post-processing parameters
    auto& ssr = pp.getSSR();
    ssr.config_.maxSteps = 64;
    ssr.config_.maxBinarySteps = 8;
    ssr.config_.maxDistance = 150.0f;
    ssr.config_.useTemporalReprojection = true;

    auto& bloom = pp.getBloom();
    bloom.config_.threshold = 1.0f;
    bloom.config_.intensity = 1.2f;
    bloom.config_.iterations = 4;

    auto& atmosphere = pp.getAtmosphere();
    atmosphere.config_.fogColor = glm::vec3(0.6f, 0.7f, 0.85f);
    atmosphere.config_.fogDensity = 0.02f;
    atmosphere.config_.useHeightFog = true;

    auto& sunFx = pp.getSunEffects();
    sunFx.config_.enableGodRays = true;
    sunFx.config_.godRaySamples = 32;
    sunFx.config_.godRayIntensity = 0.65f;

    auto& grading = pp.getColorGrading();
    grading.createNeutralLUT(16);
    grading.config_.intensity = 0.9f;
    grading.config_.saturation = 1.05f;

    // Edge detection kept optional
    pp.setEffectEnabled("edge_detection", false);

    // --- Main loop ---
    bool running = true;
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (running) {
        // Platform events (input/window)
        processPlatformEvents(running, camera);

        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        // Update scene physics / animation / wind
        scene.update(dt);
        vegetation.update(dt);
        water.update(dt);

        // Update camera and matrices
        camera.update(dt);

        // 1) Render shadows (RSM + atlas) for all lights
        shadowManager.renderShadows(scene, camera);

        // 2) Forward render the scene into HDR color / G-buffer attachments
        forwardRenderer.renderScene(scene, camera);

        // Retrieve G-Buffer textures from the renderer
        GLuint colorTex = forwardRenderer.getColorTexture();
        GLuint normalTex = forwardRenderer.getNormalTexture();
        GLuint depthTex = forwardRenderer.getDepthTexture();
        GLuint positionTex = forwardRenderer.getPositionTexture();

        // 3) Post-processing (SSR, Bloom, Atmosphere, SunEffects, ColorGrading, EdgeDetection)
        glm::vec2 sunScreen = camera.worldToScreen(-sun->getDirection());
        GLuint finalTex = pp.processFrame(colorTex, normalTex, depthTex, positionTex,
                                         camera.getProjection(), camera.getView(), camera.getLastViewProjection(), sunScreen);

        // 4) Tone map & present finalTex to screen (forwardRenderer has helper)
        forwardRenderer.presentFinal(finalTex);

        // Simple debug keys
        if (isKeyPressed(KEY_F1)) pp.setEffectEnabled("ssr", !pp.isEffectEnabled("ssr"));
        if (isKeyPressed(KEY_F2)) pp.setEffectEnabled("bloom", !pp.isEffectEnabled("bloom"));
        if (isKeyPressed(KEY_F3)) pp.setEffectEnabled("atmosphere", !pp.isEffectEnabled("atmosphere"));

        // Swap buffers
        swapPlatformBuffers();
    }

    // Cleanup
    pp.~PostProcessingPipeline();
    shadowManager.~ShadowManager();
    forwardRenderer.shutdown();
    shutdownPlatform();

    return 0;
}

// -----------------------------
// Platform/engine helper stubs
// -----------------------------

// The demo uses the following helper stubs. Replace these with your engine
// platform implementations (GLFW/SDL windowing, input, asset loaders):

bool initializePlatform() {
    // init window/context, glew, GL state
    return true;
}

void shutdownPlatform() {
}

void processPlatformEvents(bool &running, Camera &camera) {
    // Poll input, update camera from WASD/mouse
    // Example: if escape pressed -> running = false
}

bool isKeyPressed(int key) {
    // Map to platform input polling
    return false;
}

void swapPlatformBuffers() {
}

// Placeholder asset loaders
GLuint loadTexture(const char* path) {
    // Use your texture loader (stb_image or engine-specific)
    return 0;
}

Mesh loadMesh(const char* path) {
    // Use your model loader
    return Mesh();
}

