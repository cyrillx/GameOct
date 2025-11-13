#include "RenderLib/PostProcessingPipeline.hpp"
#include "RenderLib/Shader.hpp"

namespace RenderLib {

PostProcessingPipeline::PostProcessingPipeline() = default;

PostProcessingPipeline::~PostProcessingPipeline() {
    destroy_();
}

bool PostProcessingPipeline::init(int width, int height) {
    width_ = width;
    height_ = height;

    // Initialize all systems
    if (!ssr_.init(width, height)) return false;
    if (!bloom_.init(width, height)) return false;
    if (!parallax_.init()) return false;
    if (!atmosphere_.init(width, height)) return false;
    if (!sunEffects_.init(width, height)) return false;
    if (!colorGrading_.init()) return false;
    if (!edgeDetection_.init(width, height)) return false;

    // Create final composite
    createFinalComposite_();

    return true;
}

void PostProcessingPipeline::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    width_ = width;
    height_ = height;

    ssr_.resize(width, height);
    bloom_.resize(width, height);
    atmosphere_.resize(width, height);
    sunEffects_.resize(width, height);
    edgeDetection_.resize(width, height);

    // Resize output textures
    if (outputTex_) glDeleteTextures(1, &outputTex_);
    if (compositeTex_) glDeleteTextures(1, &compositeTex_);

    glGenTextures(1, &outputTex_);
    glBindTexture(GL_TEXTURE_2D, outputTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &compositeTex_);
    glBindTexture(GL_TEXTURE_2D, compositeTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (outputFBO_) {
        glBindFramebuffer(GL_FRAMEBUFFER, outputFBO_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex_, 0);
    }
}

GLuint PostProcessingPipeline::processFrame(GLuint colorTex, GLuint normalTex, GLuint depthTex, 
                                            GLuint positionTex, const glm::mat4& proj, 
                                            const glm::mat4& view, const glm::mat4& lastViewProj,
                                            const glm::vec2& sunScreenPos) {
    GLuint currentColor = colorTex;
    GLuint currentResult = colorTex;

    // 1. Screen Space Reflections (affects lighting)
    if (enableSSR_) {
        ssr_.render(currentColor, normalTex, depthTex, proj, view, lastViewProj);
        currentResult = ssr_.getReflectionTexture();
    }

    // 2. Bloom (brightens highlights)
    if (enableBloom_) {
        bloom_.render(currentResult);
        currentResult = bloom_.getBloomTexture();
    }

    // 3. Atmospheric Fog (depth perception)
    if (enableAtmosphere_) {
        atmosphere_.applyFog(currentResult, depthTex, view);
        currentResult = atmosphere_.getOutputTexture();
    }

    // 4. Sun Effects (volumetric + flare)
    if (enableSunEffects_) {
        sunEffects_.renderGodRays(currentResult, depthTex);
        sunEffects_.renderLensFlare(sunScreenPos);
        currentResult = sunEffects_.getOutputTexture();
    }

    // 5. Edge Detection (optional outlines)
    if (enableEdgeDetection_) {
        edgeDetection_.detectEdges(depthTex, normalTex);
        edgeDetection_.compositeWithColor(currentResult, 0.5f);
        currentResult = edgeDetection_.getCompositeTexture();
    }

    // 6. Color Grading (cinematic final touch)
    if (enableColorGrading_) {
        colorGrading_.apply(currentResult, outputTex_);
        return outputTex_;
    }

    return currentResult;
}

void PostProcessingPipeline::createFinalComposite_() {
    compositeShader_ = std::make_shared<Shader>(
        "shaders/composite.vert",
        "shaders/composite.frag"
    );
}

void PostProcessingPipeline::setEffectEnabled(const std::string& name, bool enabled) {
    if (name == "ssr") enableSSR_ = enabled;
    else if (name == "bloom") enableBloom_ = enabled;
    else if (name == "parallax") enableParallax_ = enabled;
    else if (name == "atmosphere") enableAtmosphere_ = enabled;
    else if (name == "sun") enableSunEffects_ = enabled;
    else if (name == "color_grading") enableColorGrading_ = enabled;
    else if (name == "edge_detection") enableEdgeDetection_ = enabled;
}

bool PostProcessingPipeline::isEffectEnabled(const std::string& name) const {
    if (name == "ssr") return enableSSR_;
    if (name == "bloom") return enableBloom_;
    if (name == "parallax") return enableParallax_;
    if (name == "atmosphere") return enableAtmosphere_;
    if (name == "sun") return enableSunEffects_;
    if (name == "color_grading") return enableColorGrading_;
    if (name == "edge_detection") return enableEdgeDetection_;
    return false;
}

void PostProcessingPipeline::resetToDefaults() {
    // Reset all effect configurations to defaults
    ssr_.config_ = ScreenSpaceReflections::Config();
    bloom_.config_ = BloomPostProcess::Config();
    atmosphere_.config_ = AtmosphereRenderer::Config();
    sunEffects_.config_ = SunEffects::Config();
    colorGrading_.config_ = ColorGrading::Config();
    edgeDetection_.config_ = EdgeDetection::Config();

    // Re-enable all by default
    enableSSR_ = true;
    enableBloom_ = true;
    enableParallax_ = true;
    enableAtmosphere_ = true;
    enableSunEffects_ = true;
    enableColorGrading_ = true;
    enableEdgeDetection_ = false;
}

void PostProcessingPipeline::destroy_() {
    if (outputFBO_) glDeleteFramebuffers(1, &outputFBO_);
    if (outputTex_) glDeleteTextures(1, &outputTex_);
    if (compositeTex_) glDeleteTextures(1, &compositeTex_);
}

} // namespace RenderLib
