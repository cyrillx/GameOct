#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace RenderLib {

class Texture3D;
class Shader;

/**
 * @brief Color Grading using 3D LUT (Look-Up Table)
 * 
 * Applies cinematic color grading to the final image.
 * Uses 3D LUT for efficient color mapping.
 * 
 * LUT format: 16x16x16 3D texture (or configurable size)
 * Can be created in Photoshop, Nuke, DaVinci Resolve, etc.
 */
class ColorGrading {
public:
    struct Config {
        float intensity = 1.0f;         // Blend factor [0, 1]
        float contrast = 1.0f;          // Pre-LUT contrast adjustment
        float saturation = 1.0f;        // Pre-LUT saturation
        float brightness = 0.0f;        // Pre-LUT brightness offset
        bool useNeutralLUT = true;      // Use identity LUT if none loaded
    };

    ColorGrading();
    ~ColorGrading();

    /**
     * Load 3D LUT from file
     * Supported formats: PNG (vertical layout), TGA, raw binary
     * @param path - Path to LUT texture file
     * @param size - LUT size (default 16, so 16x16x16)
     */
    bool loadLUT(const std::string& path, int size = 16);

    /**
     * Create neutral LUT (identity mapping)
     */
    void createNeutralLUT(int size = 16);

    /**
     * Apply color grading to render target
     * @param colorTex - Input HDR or LDR texture
     * @param output - Output texture (can be same as input for in-place)
     */
    void apply(GLuint colorTex, GLuint output = 0);

    /**
     * Get current LUT texture
     */
    GLuint getLUT() const { return lutTexture_; }

    /**
     * Configure grading
     */
    void setConfig(const Config& config) { config_ = config; }
    Config getConfig() const { return config_; }

    /**
     * Get output framebuffer (if applying to separate target)
     */
    GLuint getOutputFBO() const { return gradingFBO_; }
    GLuint getOutputTexture() const { return gradingTex_; }

private:
    void createFramebuffers_(int width, int height);
    GLuint loadLUTFromFile_(const std::string& path, int& outSize);

    GLuint lutTexture_ = 0;            // 3D LUT texture
    GLuint gradingFBO_ = 0;
    GLuint gradingTex_ = 0;
    
    std::shared_ptr<Shader> gradingShader_;
    
    Config config_;
    int lutSize_ = 16;
    GLuint quadVAO_ = 0;
};

} // namespace RenderLib
