#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {

class Shader;
class Texture2D;

/**
 * @brief Screen Space Reflections (SSR)
 * 
 * Renders realistic reflections using ray-marching in screen space.
 * Works on any surface with proper depth and normal buffers.
 * 
 * Algorithm:
 * 1. For each pixel, generate a ray from camera through surface
 * 2. March the ray through depth buffer
 * 3. Check for intersection with geometry
 * 4. Blend with surface color for reflection
 */
class ScreenSpaceReflections {
public:
    struct Config {
        int maxSteps = 64;              // Ray marching iterations
        int maxBinarySteps = 8;         // Binary search refinement
        float rayStep = 1.0f;           // Ray step length
        float maxDistance = 1000.0f;    // Maximum ray distance
        float thickness = 0.1f;         // Surface thickness
        float fade = 0.3f;              // Edge fade distance
        bool useTemporalReprojection = true;  // Temporal accumulation
        float edgeFade = 0.1f;          // Screen edge fade
    };

    ScreenSpaceReflections();
    ~ScreenSpaceReflections();

    bool init(int width, int height);
    void resize(int width, int height);
    void destroy();

    /**
     * Render SSR to internal texture
     * @param colorTex - Scene color texture
     * @param normalTex - Surface normals (from G-buffer or render)
     * @param depthTex - Scene depth texture
     * @param proj - Projection matrix
     * @param view - View matrix
     * @param lastViewProj - Previous frame view-projection (for reprojection)
     */
    void render(GLuint colorTex, GLuint normalTex, GLuint depthTex,
                const glm::mat4& proj, const glm::mat4& view,
                const glm::mat4& lastViewProj);

    /**
     * Get output reflection texture
     */
    GLuint getReflectionTexture() const { return reflectionTex_; }
    
    /**
     * Configure quality
     */
    void setConfig(const Config& config) { config_ = config; }
    Config getConfig() const { return config_; }

    /**
     * Enable/disable temporal reprojection
     */
    void setTemporalReprojection(bool enable) { config_.useTemporalReprojection = enable; }

private:
    void createFramebuffers_();
    void renderRayMarch_();
    void applyTemporalReprojection_();

    GLuint ssrFBO_ = 0;
    GLuint reflectionTex_ = 0;
    GLuint reflectionPrevTex_ = 0;
    GLuint rayMarchTex_ = 0;
    
    std::shared_ptr<Shader> rayMarchShader_;
    std::shared_ptr<Shader> temporalShader_;
    
    Config config_;
    int width_ = 0;
    int height_ = 0;
    GLuint quadVAO_ = 0;
};

} // namespace RenderLib
