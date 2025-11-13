#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "Mesh.hpp"
#include "Texture.hpp"

namespace RenderLib {

/**
 * @brief Heightfield-based terrain with quad-tree LOD subdivision.
 * 
 * Divides terrain into tiles (32x32 vertices each).
 * Each tile has 5 LOD levels: 32x32 -> 16x16 -> 8x8 -> 4x4 -> 2x2.
 * LOD selection based on distance from camera.
 */
class Terrain {
public:
    struct Config {
        uint32_t gridWidth = 256;      // Total terrain width in vertices
        uint32_t gridHeight = 256;     // Total terrain height in vertices
        float tileSize = 1.0f;         // Physical size of each vertex spacing
        float maxHeight = 100.0f;      // Maximum height for heightmap normalization
        uint32_t tileResolution = 32;  // Vertices per tile (32x32)
    };

    struct Tile {
        std::unique_ptr<Mesh> meshes[5];  // 5 LOD levels: full -> decimated
        glm::vec3 aabbMin, aabbMax;       // Bounding box for culling
        glm::vec2 gridPos;                // Position in grid (tiles, not vertices)
        uint32_t vertexCounts[5];         // Vertex count per LOD
    };

    Terrain();
    ~Terrain();

    /**
     * @brief Generate procedural heightfield from noise or loaded heightmap
     */
    bool generateFromHeightmap(const Config& config, const std::vector<float>& heightmap);
    bool generateFromNoise(const Config& config);

    /**
     * @brief Set heightmap texture (for displacement or visualization)
     */
    void setHeightmapTexture(std::shared_ptr<Texture2D> tex) { heightmapTex_ = tex; }
    void setDiffuseTexture(std::shared_ptr<Texture2D> tex) { diffuseTex_ = tex; }
    void setNormalTexture(std::shared_ptr<Texture2D> tex) { normalTex_ = tex; }

    /**
     * @brief Get texture
     */
    std::shared_ptr<Texture2D> getHeightmapTexture() const { return heightmapTex_; }
    std::shared_ptr<Texture2D> getDiffuseTexture() const { return diffuseTex_; }
    std::shared_ptr<Texture2D> getNormalTexture() const { return normalTex_; }

    /**
     * @brief Select LOD for each tile based on distance to camera
     */
    void updateLOD(const glm::vec3& cameraPos);

    /**
     * @brief Render all tiles with selected LODs
     */
    void render() const;

    /**
     * @brief Get number of tiles
     */
    uint32_t getTileCount() const { return static_cast<uint32_t>(tiles_.size()); }

    /**
     * @brief Get tile by index
     */
    Tile* getTile(uint32_t idx) { return idx < tiles_.size() ? &tiles_[idx] : nullptr; }

    /**
     * @brief Get height at world position (interpolated)
     */
    float getHeightAt(float worldX, float worldZ) const;

    /**
     * @brief Config getter
     */
    const Config& getConfig() const { return config_; }

private:
    void createTiles_(const std::vector<float>& heightmap);
    void createTileMeshes_(uint32_t tileIdx, const std::vector<float>& heightmap);
    Mesh* createLODMesh_(uint32_t tileIdx, uint32_t lodLevel, const std::vector<float>& heightmap);
    uint32_t selectLOD_(float distance) const;

    Config config_;
    std::vector<Tile> tiles_;
    std::vector<float> heightfield_;

    std::shared_ptr<Texture2D> heightmapTex_;
    std::shared_ptr<Texture2D> diffuseTex_;
    std::shared_ptr<Texture2D> normalTex_;
};

} // namespace RenderLib
