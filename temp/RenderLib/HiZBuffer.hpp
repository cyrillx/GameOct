#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

namespace RenderLib {

/**
 * @brief Hierarchical Z-buffer for occlusion culling
 * 
 * Build a pyramid of depth textures (each level is 1/2 size of previous).
 * Test AABB visibility by checking against mip levels.
 */
class HiZBuffer {
public:
    struct Level {
        uint32_t width, height;
        std::vector<float> depths;
    };

    HiZBuffer();
    ~HiZBuffer();

    /**
     * @brief Initialize Hi-Z from base depth texture
     * @param depthBuffer Pointer to depth data (single float per pixel, range [0, 1])
     * @param width Base width
     * @param height Base height
     */
    void build(const float* depthBuffer, uint32_t width, uint32_t height);

    /**
     * @brief Test if AABB is visible (not occluded)
     * Uses conservative test: AABB visible if any part might be visible
     */
    bool isVisible(const glm::vec3& aabbMin, const glm::vec3& aabbMax,
                   const glm::mat4& projection, const glm::mat4& view) const;

    /**
     * @brief Get number of mip levels
     */
    uint32_t getLevelCount() const { return static_cast<uint32_t>(pyramid_.size()); }

    /**
     * @brief Get specific level
     */
    const Level& getLevel(uint32_t idx) const { return pyramid_[idx]; }

private:
    std::vector<Level> pyramid_;

    glm::vec2 projectToScreen_(const glm::vec3& worldPos,
                               const glm::mat4& projection,
                               const glm::mat4& view) const;
};

} // namespace RenderLib
