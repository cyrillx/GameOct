#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <cstdint>

namespace RenderLib {

class Mesh;
class Texture2D;
class Shader;

/**
 * @brief Imposter/billboard generation and management
 * 
 * Creates 2D sprite representations of 3D models for very distant LOD.
 * Supports 8 angles (octahedral billboards) for 3D appearance.
 * 
 * Performance: Single imposter = 4-6 triangles vs 1000+ for full mesh
 */
class ImposterGenerator {
public:
    struct ImposterConfig {
        uint32_t angles = 8;                    // 4 or 8 viewpoints
        uint32_t spriteResolution = 512;        // Resolution of single sprite
        bool includeNormal = true;
        bool includeDepth = true;
    };

    ImposterGenerator();
    ~ImposterGenerator();

    // Generate imposters from mesh (offline process)
    bool generateFromMesh(const Mesh* mesh, const ImposterConfig& config);

    // Save imposters to file (DDS or KTX2 format)
    bool saveToFile(const std::string& filepath);

    // Load imposters from file
    bool loadFromFile(const std::string& filepath);

    // Get generated impostor texture atlas
    std::shared_ptr<Texture2D> getAlbedoTexture() const { return albedoTexture_; }
    std::shared_ptr<Texture2D> getNormalTexture() const { return normalTexture_; }
    std::shared_ptr<Texture2D> getDepthTexture() const { return depthTexture_; }

    uint32_t getAngleCount() const { return config_.angles; }
    uint32_t getSpriteResolution() const { return config_.spriteResolution; }

private:
    ImposterConfig config_;

    // Result textures (array or atlas)
    std::shared_ptr<Texture2D> albedoTexture_;
    std::shared_ptr<Texture2D> normalTexture_;
    std::shared_ptr<Texture2D> depthTexture_;

    bool renderMeshToSprite_(const Mesh* mesh, uint32_t angle, 
                            std::vector<uint8_t>& albedoData,
                            std::vector<uint8_t>& normalData,
                            std::vector<float>& depthData);

    bool createAtlasTexture_();
};

}  // namespace RenderLib
