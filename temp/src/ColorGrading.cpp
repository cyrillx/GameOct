#include "RenderLib/ColorGrading.hpp"
#include "RenderLib/Shader.hpp"
#include <iostream>

namespace RenderLib {

ColorGrading::ColorGrading() = default;

ColorGrading::~ColorGrading() {
    destroy();
}

bool ColorGrading::loadLUT(const std::string& path, int size) {
    lutSize_ = size;
    GLuint lut = loadLUTFromFile_(path, lutSize_);
    
    if (lut == 0) {
        // Fall back to neutral LUT
        std::cerr << "Failed to load LUT: " << path << ", using neutral LUT\n";
        createNeutralLUT(size);
        return false;
    }

    if (lutTexture_) {
        glDeleteTextures(1, &lutTexture_);
    }
    lutTexture_ = lut;
    return true;
}

void ColorGrading::createNeutralLUT(int size) {
    lutSize_ = size;
    
    // Create identity LUT
    std::vector<float> data(size * size * size * 4);
    
    for (int z = 0; z < size; ++z) {
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                int idx = ((z * size + y) * size + x) * 4;
                data[idx + 0] = x / static_cast<float>(size - 1);
                data[idx + 1] = y / static_cast<float>(size - 1);
                data[idx + 2] = z / static_cast<float>(size - 1);
                data[idx + 3] = 1.0f;
            }
        }
    }

    if (lutTexture_) {
        glDeleteTextures(1, &lutTexture_);
    }

    glGenTextures(1, &lutTexture_);
    glBindTexture(GL_TEXTURE_3D, lutTexture_);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, size, size, size, 0, GL_RGBA, GL_FLOAT, data.data());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void ColorGrading::apply(GLuint colorTex, GLuint output) {
    if (!gradingShader_) {
        return;
    }

    GLuint targetFBO = output == 0 ? gradingFBO_ : 0;
    GLuint targetTex = output == 0 ? gradingTex_ : output;

    glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
    if (targetFBO != 0) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTex, 0);
    }

    gradingShader_->use();
    gradingShader_->setInt("uColorTex", 0);
    gradingShader_->setInt("uLUT", 1);
    gradingShader_->setFloat("uIntensity", config_.intensity);
    gradingShader_->setFloat("uContrast", config_.contrast);
    gradingShader_->setFloat("uSaturation", config_.saturation);
    gradingShader_->setFloat("uBrightness", config_.brightness);
    gradingShader_->setFloat("uLUTSize", static_cast<float>(lutSize_));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, lutTexture_);

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint ColorGrading::loadLUTFromFile_(const std::string& path, int& outSize) {
    // Simplified - would need actual image loading (stb_image or similar)
    // For now, create neutral LUT
    outSize = 16;
    return 0;  // Return 0 to indicate neutral LUT should be used
}

} // namespace RenderLib
