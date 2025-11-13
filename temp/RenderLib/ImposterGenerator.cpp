#include "RenderLib/ImposterGenerator.hpp"
#include "RenderLib/Mesh.hpp"
#include "RenderLib/Texture2D.hpp"
#include "RenderLib/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

namespace RenderLib {

ImposterGenerator::ImposterGenerator() {}

ImposterGenerator::~ImposterGenerator() {}

bool ImposterGenerator::generateFromMesh(const Mesh* mesh, const ImposterConfig& config) {
    if (!mesh) return false;
    
    config_ = config;
    
    // Create FBO for rendering sprites
    GLuint fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, config_.spriteResolution, config_.spriteResolution);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Imposter FBO incomplete\n";
        glDeleteFramebuffers(1, &fbo);
        glDeleteRenderbuffers(1, &rbo);
        return false;
    }
    
    // Prepare data storage for all angles
    std::vector<std::vector<uint8_t>> albedoData(config_.angles);
    std::vector<std::vector<uint8_t>> normalData(config_.angles);
    std::vector<std::vector<float>> depthData(config_.angles);
    
    // Render each angle
    for (uint32_t angle = 0; angle < config_.angles; ++angle) {
        glViewport(0, 0, config_.spriteResolution, config_.spriteResolution);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Setup camera for this angle
        float theta = (2.0f * 3.14159f * angle) / config_.angles;
        glm::vec3 camPos = glm::vec3(std::cos(theta), 1.0f, std::sin(theta)) * 10.0f;
        
        // Render mesh from this angle
        renderMeshToSprite_(mesh, angle, albedoData[angle], 
                           normalData[angle], depthData[angle]);
    }
    
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    
    // Create atlas texture
    createAtlasTexture_();
    
    return true;
}

bool ImposterGenerator::saveToFile(const std::string& filepath) {
    // TODO: Implement DDS/KTX2 saving
    // For now, just validate that we have texture data
    return albedoTexture_ != nullptr;
}

bool ImposterGenerator::loadFromFile(const std::string& filepath) {
    // TODO: Implement DDS/KTX2 loading
    return false;
}

bool ImposterGenerator::renderMeshToSprite_(const Mesh* mesh, uint32_t angle,
                                          std::vector<uint8_t>& albedoData,
                                          std::vector<uint8_t>& normalData,
                                          std::vector<float>& depthData) {
    // Allocate storage
    size_t pixelCount = config_.spriteResolution * config_.spriteResolution;
    albedoData.resize(pixelCount * 4);
    if (config_.includeNormal) normalData.resize(pixelCount * 4);
    if (config_.includeDepth) depthData.resize(pixelCount);
    
    // Render mesh at this angle
    // This would use a special "imposter" shader
    
    return true;
}

bool ImposterGenerator::createAtlasTexture_() {
    uint32_t atlasResolution = config_.spriteResolution * config_.angles;
    
    albedoTexture_ = std::make_shared<Texture2D>();
    
    glGenTextures(1, &albedoTexture_->textureID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, albedoTexture_->textureID);
    
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
                 config_.spriteResolution, config_.spriteResolution, config_.angles,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return true;
}

}  // namespace RenderLib
