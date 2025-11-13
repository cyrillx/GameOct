#include "RenderLib/Terrain.hpp"
#include <glm/gtc/noise.hpp>
#include <algorithm>
#include <cmath>

namespace RenderLib {

Terrain::Terrain() = default;
Terrain::~Terrain() = default;

bool Terrain::generateFromHeightmap(const Config& config, const std::vector<float>& heightmap) {
    config_ = config;
    heightfield_ = heightmap;

    if (heightmap.size() != config.gridWidth * config.gridHeight) {
        return false;  // Invalid heightmap size
    }

    createTiles_(heightmap);
    return !tiles_.empty();
}

bool Terrain::generateFromNoise(const Config& config) {
    config_ = config;
    heightfield_.resize(config.gridWidth * config.gridHeight);

    // Simple Perlin noise-based heightfield
    for (uint32_t z = 0; z < config.gridHeight; ++z) {
        for (uint32_t x = 0; x < config.gridWidth; ++x) {
            float nx = (float)x / config.gridWidth * 4.0f;
            float nz = (float)z / config.gridHeight * 4.0f;

            // Perlin noise (requires external library or simple implementation)
            float height = glm::perlin(glm::vec2(nx, nz)) * 0.5f + 0.5f;
            heightfield_[z * config.gridWidth + x] = height * config.maxHeight;
        }
    }

    createTiles_(heightfield_);
    return !tiles_.empty();
}

void Terrain::createTiles_(const std::vector<float>& heightmap) {
    tiles_.clear();
    uint32_t tilesX = (config_.gridWidth - 1) / (config_.tileResolution - 1);
    uint32_t tilesZ = (config_.gridHeight - 1) / (config_.tileResolution - 1);

    tiles_.resize(tilesX * tilesZ);

    for (uint32_t tz = 0; tz < tilesZ; ++tz) {
        for (uint32_t tx = 0; tx < tilesX; ++tx) {
            uint32_t tileIdx = tz * tilesX + tx;
            tiles_[tileIdx].gridPos = glm::vec2(tx, tz);
            createTileMeshes_(tileIdx, heightmap);
        }
    }
}

void Terrain::createTileMeshes_(uint32_t tileIdx, const std::vector<float>& heightmap) {
    Tile& tile = tiles_[tileIdx];

    // Create 5 LOD levels: full -> 2x2
    uint32_t lodReductions[5] = {1, 2, 4, 8, 16};  // Decimation factors

    for (uint32_t lod = 0; lod < 5; ++lod) {
        tile.meshes[lod].reset(createLODMesh_(tileIdx, lod, heightmap));
    }

    // Compute AABB from full LOD (LOD 0)
    if (tile.meshes[0]) {
        tile.aabbMin = glm::vec3(-1000, -1000, -1000);
        tile.aabbMax = glm::vec3(1000, 1000, 1000);
        // TODO: properly compute from vertices
    }
}

Mesh* Terrain::createLODMesh_(uint32_t tileIdx, uint32_t lodLevel, const std::vector<float>& heightmap) {
    uint32_t tilesX = (config_.gridWidth - 1) / (config_.tileResolution - 1);
    uint32_t tx = tileIdx % tilesX;
    uint32_t tz = tileIdx / tilesX;

    uint32_t step = 1u << lodLevel;  // 1, 2, 4, 8, 16
    uint32_t resolution = (config_.tileResolution - 1) / step + 1;

    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t startX = tx * (config_.tileResolution - 1);
    uint32_t startZ = tz * (config_.tileResolution - 1);

    vertices.reserve(resolution * resolution);

    // Generate vertices
    for (uint32_t z = 0; z < resolution; ++z) {
        for (uint32_t x = 0; x < resolution; ++x) {
            uint32_t gridX = startX + x * step;
            uint32_t gridZ = startZ + z * step;

            if (gridX >= config_.gridWidth || gridZ >= config_.gridHeight) continue;

            float heightValue = heightmap[gridZ * config_.gridWidth + gridX];

            glm::vec3 pos(
                (gridX - config_.gridWidth / 2.0f) * config_.tileSize,
                heightValue,
                (gridZ - config_.gridHeight / 2.0f) * config_.tileSize
            );

            glm::vec3 normal(0, 1, 0);  // Placeholder; compute from neighbors

            // Compute proper normal from heightfield
            float hLeft = heightmap[std::max(0, (int)gridZ * (int)config_.gridWidth + (int)gridX - 1)];
            float hRight = heightmap[std::min((int)(config_.gridWidth - 1), (int)gridX + 1) + (int)gridZ * (int)config_.gridWidth];
            float hUp = heightmap[std::max(0, (int)gridZ - 1) * (int)config_.gridWidth + (int)gridX];
            float hDown = heightmap[std::min((int)(config_.gridHeight - 1), (int)gridZ + 1) * (int)config_.gridWidth + (int)gridX];

            glm::vec3 tangentX(config_.tileSize * 2.0f, hRight - hLeft, 0.0f);
            glm::vec3 tangentZ(0.0f, hDown - hUp, config_.tileSize * 2.0f);
            normal = glm::normalize(glm::cross(tangentZ, tangentX));

            glm::vec2 texCoord(x / (float)(resolution - 1), z / (float)(resolution - 1));

            vertices.push_back({pos, normal, texCoord, glm::vec3(1, 0, 0)});
        }
    }

    // Generate indices (triangle strip optimization)
    for (uint32_t z = 0; z < resolution - 1; ++z) {
        for (uint32_t x = 0; x < resolution - 1; ++x) {
            uint32_t a = z * resolution + x;
            uint32_t b = a + 1;
            uint32_t c = (z + 1) * resolution + x;
            uint32_t d = c + 1;

            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(b);

            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    Mesh* mesh = new Mesh();
    if (!mesh->create(vertices, indices)) {
        delete mesh;
        return nullptr;
    }

    return mesh;
}

void Terrain::updateLOD(const glm::vec3& cameraPos) {
    // Placeholder: in real implementation, would select LOD per tile
    // based on distance to camera
}

void Terrain::render() const {
    for (const auto& tile : tiles_) {
        // Render selected LOD (placeholder: render LOD 0)
        if (tile.meshes[0]) {
            tile.meshes[0]->draw();
        }
    }
}

float Terrain::getHeightAt(float worldX, float worldZ) const {
    // Convert world position to grid coordinates
    float gridX = worldX / config_.tileSize + config_.gridWidth / 2.0f;
    float gridZ = worldZ / config_.tileSize + config_.gridHeight / 2.0f;

    if (gridX < 0 || gridX >= config_.gridWidth || gridZ < 0 || gridZ >= config_.gridHeight) {
        return 0.0f;
    }

    // Bilinear interpolation
    uint32_t x0 = (uint32_t)gridX;
    uint32_t z0 = (uint32_t)gridZ;
    uint32_t x1 = std::min(x0 + 1, config_.gridWidth - 1);
    uint32_t z1 = std::min(z0 + 1, config_.gridHeight - 1);

    float fx = gridX - x0;
    float fz = gridZ - z0;

    float h00 = heightfield_[z0 * config_.gridWidth + x0];
    float h10 = heightfield_[z0 * config_.gridWidth + x1];
    float h01 = heightfield_[z1 * config_.gridWidth + x0];
    float h11 = heightfield_[z1 * config_.gridWidth + x1];

    float h0 = h00 * (1 - fx) + h10 * fx;
    float h1 = h01 * (1 - fx) + h11 * fx;

    return h0 * (1 - fz) + h1 * fz;
}

} // namespace RenderLib
