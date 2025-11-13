/**
 * @file PostProcessingPipeline.hpp
 * @brief Complete post-processing pipeline integrating all 7 new rendering systems
 *
 * This file demonstrates how to integrate all post-processing systems into
 * a unified rendering pipeline suitable for quality FPS games.
 *
 * Integration Pattern:
 * 1. Initialize all systems after main renderer setup
 * 2. Render scene to G-Buffer (color, normal, depth, etc.)
 * 3. Apply post-processing effects in order
 * 4. Composite final result to screen
 */

#pragma once

#include "ScreenSpaceReflections.hpp"
#include "BloomPostProcess.hpp"
#include "ParallaxMapper.hpp"
#include "AtmosphereRenderer.hpp"
#include "SunEffects.hpp"
#include "ColorGrading.hpp"
#include "EdgeDetection.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <GL/glew.h>

namespace RenderLib {

/**
 * Post-processing effect order (typical FPS workflow)
 * 1. SSR (affects lighting)
 * 2. Bloom (brightens highlights)
 * 3. Atmosphere Fog (depth perception)
 * 4. Sun Effects (volumetric/flare)
 * 5. Edge Detection (optional outlines)
 * 6. Color Grading (cinematic final touch)
 */
class PostProcessingPipeline {
public:
    PostProcessingPipeline();
    ~PostProcessingPipeline();

    /**
     * Initialize all post-processing systems
     * @param width Render target width
     * @param height Render target height
     * @return true if all systems initialized successfully
     */
    bool init(int width, int height);

    /**
     * Resize all framebuffers
     * @param width New width
     * @param height New height
     */
    void resize(int width, int height);

    /**
     * Main post-processing pass
     * Expects G-Buffer inputs:
     * - colorTex: Scene color (from forward/deferred rendering)
     * - normalTex: World space normals
     * - depthTex: Linear depth (0-1)
     * - positionTex: World space positions (for SSR)
     *
     * @param colorTex Input scene color texture
     * @param normalTex G-Buffer normal map
     * @param depthTex G-Buffer depth map
     * @param positionTex G-Buffer position map
     * @param proj Projection matrix
     * @param view View matrix
     * @param lastViewProj Previous frame's view-projection matrix (for SSR temporal)
     * @param sunScreenPos 2D screen position of sun (for lens flare)
     * @return Output texture containing post-processed result
     */
    GLuint processFrame(GLuint colorTex, GLuint normalTex, GLuint depthTex, GLuint positionTex,
                        const glm::mat4& proj, const glm::mat4& view, const glm::mat4& lastViewProj,
                        const glm::vec2& sunScreenPos);

    /**
     * Retrieve final output texture
     */
    GLuint getOutputTexture() const { return outputTex_; }

    // Configuration accessors
    ScreenSpaceReflections& getSSR() { return ssr_; }
    BloomPostProcess& getBloom() { return bloom_; }
    ParallaxMapper& getParallax() { return parallax_; }
    AtmosphereRenderer& getAtmosphere() { return atmosphere_; }
    SunEffects& getSunEffects() { return sunEffects_; }
    ColorGrading& getColorGrading() { return colorGrading_; }
    EdgeDetection& getEdgeDetection() { return edgeDetection_; }

    /**
     * Enable/disable individual effects without reinitializing
     */
    void setEffectEnabled(const std::string& name, bool enabled);
    bool isEffectEnabled(const std::string& name) const;

    /**
     * Reset all effects to default configuration
     */
    void resetToDefaults();

private:
    void createFinalComposite_();
    void compositeFinal_();
    void destroy_();

    int width_ = 0;
    int height_ = 0;

    // Post-processing systems
    ScreenSpaceReflections ssr_;
    BloomPostProcess bloom_;
    ParallaxMapper parallax_;
    AtmosphereRenderer atmosphere_;
    SunEffects sunEffects_;
    ColorGrading colorGrading_;
    EdgeDetection edgeDetection_;

    // Final composite
    GLuint outputFBO_ = 0;
    GLuint outputTex_ = 0;
    GLuint compositeTex_ = 0;

    std::shared_ptr<Shader> compositeShader_;

    // Effect enable flags
    bool enableSSR_ = true;
    bool enableBloom_ = true;
    bool enableParallax_ = true;
    bool enableAtmosphere_ = true;
    bool enableSunEffects_ = true;
    bool enableColorGrading_ = true;
    bool enableEdgeDetection_ = false;
};

} // namespace RenderLib
