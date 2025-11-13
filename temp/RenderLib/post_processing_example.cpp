/**
 * @file post_processing_example.cpp
 * @brief Complete example demonstrating how to use the post-processing pipeline
 *        in a typical FPS game rendering loop
 *
 * This example shows:
 * 1. Initialization of the pipeline
 * 2. Integration into main render loop
 * 3. Configuration of individual effects
 * 4. Real-time effect toggling
 */

#include "RenderLib/ForwardRenderer.hpp"
#include "RenderLib/PostProcessingPipeline.hpp"
#include "RenderLib/Camera.hpp"

using namespace RenderLib;

class FPSGameRenderer {
private:
    ForwardRenderer renderer_;
    PostProcessingPipeline postProcessing_;
    
    // G-Buffer textures
    GLuint gbufferColor_ = 0;
    GLuint gbufferNormal_ = 0;
    GLuint gbufferDepth_ = 0;
    GLuint gbufferPosition_ = 0;
    GLuint gbufferFBO_ = 0;

    int viewportWidth_ = 1920;
    int viewportHeight_ = 1080;

public:
    bool initialize() {
        // Initialize renderer
        if (!renderer_.init(viewportWidth_, viewportHeight_)) {
            return false;
        }

        // Initialize post-processing pipeline
        if (!postProcessing_.init(viewportWidth_, viewportHeight_)) {
            return false;
        }

        // Create G-Buffer for deferred-ish rendering
        createGBuffer_();

        // Configure SSR
        auto& ssr = postProcessing_.getSSR();
        ssr.config_.maxSteps = 64;
        ssr.config_.maxBinarySteps = 8;
        ssr.config_.maxDistance = 100.0f;
        ssr.config_.rayStep = 0.5f;
        ssr.config_.useTemporalReprojection = true;

        // Configure Bloom
        auto& bloom = postProcessing_.getBloom();
        bloom.config_.threshold = 1.0f;
        bloom.config_.intensity = 1.5f;
        bloom.config_.iterations = 5;

        // Configure Atmosphere
        auto& atmosphere = postProcessing_.getAtmosphere();
        atmosphere.config_.fogColor = glm::vec3(0.7f, 0.8f, 0.9f);  // Light blue
        atmosphere.config_.fogDensity = 0.05f;
        atmosphere.config_.fogNear = 1.0f;
        atmosphere.config_.fogFar = 1000.0f;
        atmosphere.config_.fogType = AtmosphereRenderer::FogType::ExponentialSq;
        atmosphere.config_.useHeightFog = true;
        atmosphere.config_.sunDirection = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
        atmosphere.config_.sunColor = glm::vec3(1.0f, 0.9f, 0.7f);
        atmosphere.config_.skyScattering = 0.3f;

        // Configure Sun Effects
        auto& sunEffects = postProcessing_.getSunEffects();
        sunEffects.config_.enableGodRays = true;
        sunEffects.config_.enableLensFlare = true;
        sunEffects.config_.godRayIntensity = 0.8f;
        sunEffects.config_.godRaySamples = 32;
        sunEffects.config_.lensFlareIntensity = 0.6f;
        sunEffects.config_.sunDirection = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
        sunEffects.config_.sunColor = glm::vec3(1.0f, 0.9f, 0.7f);

        // Configure Color Grading
        auto& colorGrading = postProcessing_.getColorGrading();
        colorGrading.createNeutralLUT(16);  // Create neutral LUT first
        colorGrading.config_.intensity = 0.8f;     // 80% LUT influence
        colorGrading.config_.saturation = 1.1f;    // 10% more vibrant
        colorGrading.config_.contrast = 1.05f;     // 5% more contrast
        colorGrading.config_.brightness = 0.0f;

        // Edge Detection disabled by default (alternative rendering style)
        postProcessing_.setEffectEnabled("edge_detection", false);

        return true;
    }

    void render(const Camera& camera, const Scene& scene, float deltaTime) {
        // PHASE 1: Render scene to G-Buffer
        renderToGBuffer_(camera, scene);

        // PHASE 2: Apply post-processing pipeline
        GLuint finalColor = postProcessing_.processFrame(
            gbufferColor_,
            gbufferNormal_,
            gbufferDepth_,
            gbufferPosition_,
            camera.getProjection(),
            camera.getView(),
            camera.getLastViewProjection(),
            getSunScreenPosition_(camera)
        );

        // PHASE 3: Composite to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewportWidth_, viewportHeight_);
        renderQuad_(finalColor);
    }

    // Configuration accessors for real-time tweaking
    void toggleEffect(const std::string& effectName) {
        bool current = postProcessing_.isEffectEnabled(effectName);
        postProcessing_.setEffectEnabled(effectName, !current);
    }

    void setSSRQuality(int maxSteps, float maxDistance) {
        auto& ssr = postProcessing_.getSSR();
        ssr.config_.maxSteps = maxSteps;
        ssr.config_.maxDistance = maxDistance;
    }

    void setFogDensity(float density) {
        auto& atmosphere = postProcessing_.getAtmosphere();
        atmosphere.config_.fogDensity = density;
    }

    void setBloomThreshold(float threshold) {
        auto& bloom = postProcessing_.getBloom();
        bloom.config_.threshold = threshold;
    }

    void enableEdgeDetection(bool enable) {
        postProcessing_.setEffectEnabled("edge_detection", enable);
    }

    void resetEffectsToDefaults() {
        postProcessing_.resetToDefaults();
    }

    void resize(int width, int height) {
        viewportWidth_ = width;
        viewportHeight_ = height;
        postProcessing_.resize(width, height);
        recreateGBuffer_();
    }

    ~FPSGameRenderer() {
        destroyGBuffer_();
    }

private:
    void createGBuffer_() {
        glGenTextures(1, &gbufferColor_);
        glBindTexture(GL_TEXTURE_2D, gbufferColor_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewportWidth_, viewportHeight_, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &gbufferNormal_);
        glBindTexture(GL_TEXTURE_2D, gbufferNormal_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewportWidth_, viewportHeight_, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &gbufferDepth_);
        glBindTexture(GL_TEXTURE_2D, gbufferDepth_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, viewportWidth_, viewportHeight_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &gbufferPosition_);
        glBindTexture(GL_TEXTURE_2D, gbufferPosition_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewportWidth_, viewportHeight_, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenFramebuffers(1, &gbufferFBO_);
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbufferColor_, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbufferNormal_, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbufferPosition_, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gbufferDepth_, 0);
    }

    void renderToGBuffer_(const Camera& camera, const Scene& scene) {
        glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO_);
        glViewport(0, 0, viewportWidth_, viewportHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render scene using forward renderer
        renderer_.render(scene, camera);
    }

    void recreateGBuffer_() {
        destroyGBuffer_();
        createGBuffer_();
    }

    void destroyGBuffer_() {
        if (gbufferFBO_) glDeleteFramebuffers(1, &gbufferFBO_);
        if (gbufferColor_) glDeleteTextures(1, &gbufferColor_);
        if (gbufferNormal_) glDeleteTextures(1, &gbufferNormal_);
        if (gbufferDepth_) glDeleteTextures(1, &gbufferDepth_);
        if (gbufferPosition_) glDeleteTextures(1, &gbufferPosition_);
    }

    glm::vec2 getSunScreenPosition_(const Camera& camera) {
        // Project sun position (or light direction) to screen space
        glm::vec4 sunWorldPos = glm::vec4(glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)) * 100.0f, 1.0f);
        glm::vec4 sunClipPos = camera.getProjection() * camera.getView() * sunWorldPos;
        glm::vec2 sunScreenPos = glm::vec2(sunClipPos.x / sunClipPos.w, sunClipPos.y / sunClipPos.w) * 0.5f + 0.5f;
        return sunScreenPos;
    }

    void renderQuad_(GLuint texture) {
        // Render full-screen quad with final color
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
    }
};

/**
 * Main game loop example
 */
int main() {
    FPSGameRenderer gameRenderer;
    if (!gameRenderer.initialize()) {
        return 1;
    }

    Camera camera(glm::vec3(0, 1.7f, 5), 75.0f);
    Scene scene;

    // Main loop
    while (true) {
        // Input handling
        if (isKeyPressed('1')) gameRenderer.toggleEffect("ssr");
        if (isKeyPressed('2')) gameRenderer.toggleEffect("bloom");
        if (isKeyPressed('3')) gameRenderer.toggleEffect("atmosphere");
        if (isKeyPressed('4')) gameRenderer.toggleEffect("sun");
        if (isKeyPressed('5')) gameRenderer.enableEdgeDetection(true);
        if (isKeyPressed('R')) gameRenderer.resetEffectsToDefaults();

        // Update
        camera.update(deltaTime);
        scene.update(deltaTime);

        // Render
        gameRenderer.render(camera, scene, deltaTime);

        // Present
        swapBuffers();
    }

    return 0;
}
