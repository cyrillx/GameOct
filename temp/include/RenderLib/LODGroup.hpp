#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "Mesh.hpp"
#include "Material.hpp"

namespace RenderLib {

/**
 * @brief LOD Group manages multiple mesh representations of same object
 * 
 * LOD 0: Full detail (closest)
 * LOD 1-3: Reduced geometry
 * LOD 4: Imposter/billboard (farthest)
 */
class LODGroup {
public:
    struct LODLevel {
        std::shared_ptr<Mesh> mesh;
        float minDistance;  // Distance at which this LOD becomes active
        float maxDistance;
    };

    LODGroup();
    ~LODGroup();

    /**
     * @brief Add LOD level
     */
    void addLOD(uint32_t level, std::shared_ptr<Mesh> mesh,
                float minDist, float maxDist);

    /**
     * @brief Select LOD based on distance
     */
    Mesh* selectLOD(float distance) const;

    /**
     * @brief Get LOD info
     */
    const std::vector<LODLevel>& getLODs() const { return lods_; }

    /**
     * @brief Get count of LODs
     */
    uint32_t getLODCount() const { return static_cast<uint32_t>(lods_.size()); }

private:
    std::vector<LODLevel> lods_;
};

} // namespace RenderLib
