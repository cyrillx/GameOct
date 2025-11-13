#include "RenderLib/LODGroup.hpp"
#include <algorithm>

namespace RenderLib {

LODGroup::LODGroup() = default;
LODGroup::~LODGroup() = default;

void LODGroup::addLOD(uint32_t level, std::shared_ptr<Mesh> mesh,
                      float minDist, float maxDist) {
    if (level >= lods_.size()) {
        lods_.resize(level + 1);
    }

    lods_[level] = {mesh, minDist, maxDist};
}

Mesh* LODGroup::selectLOD(float distance) const {
    for (const auto& lod : lods_) {
        if (distance >= lod.minDistance && distance <= lod.maxDistance) {
            return lod.mesh.get();
        }
    }

    // Return farthest LOD if distance exceeds all ranges
    if (!lods_.empty()) {
        return lods_.back().mesh.get();
    }

    return nullptr;
}

} // namespace RenderLib
