#include "RenderLib/HiZBuffer.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace RenderLib {

HiZBuffer::HiZBuffer() = default;
HiZBuffer::~HiZBuffer() = default;

void HiZBuffer::build(const float* depthBuffer, uint32_t width, uint32_t height) {
    pyramid_.clear();

    // Level 0: full resolution
    Level level0;
    level0.width = width;
    level0.height = height;
    level0.depths.assign(depthBuffer, depthBuffer + width * height);
    pyramid_.push_back(level0);

    // Generate mip levels
    uint32_t currentWidth = width;
    uint32_t currentHeight = height;

    while (currentWidth > 1 || currentHeight > 1) {
        uint32_t nextWidth = std::max(1u, currentWidth / 2);
        uint32_t nextHeight = std::max(1u, currentHeight / 2);

        Level nextLevel;
        nextLevel.width = nextWidth;
        nextLevel.height = nextHeight;
        nextLevel.depths.resize(nextWidth * nextHeight);

        // Downsample using max (conservative)
        const Level& currentLevel = pyramid_.back();
        for (uint32_t y = 0; y < nextHeight; ++y) {
            for (uint32_t x = 0; x < nextWidth; ++x) {
                uint32_t srcX0 = x * 2;
                uint32_t srcY0 = y * 2;
                uint32_t srcX1 = std::min(srcX0 + 1, currentWidth - 1);
                uint32_t srcY1 = std::min(srcY0 + 1, currentHeight - 1);

                float d00 = currentLevel.depths[srcY0 * currentWidth + srcX0];
                float d10 = currentLevel.depths[srcY0 * currentWidth + srcX1];
                float d01 = currentLevel.depths[srcY1 * currentWidth + srcX0];
                float d11 = currentLevel.depths[srcY1 * currentWidth + srcX1];

                // Maximum depth (farthest) is least occluded
                float maxDepth = std::max({d00, d10, d01, d11});
                nextLevel.depths[y * nextWidth + x] = maxDepth;
            }
        }

        pyramid_.push_back(nextLevel);
        currentWidth = nextWidth;
        currentHeight = nextHeight;
    }
}

glm::vec2 HiZBuffer::projectToScreen_(const glm::vec3& worldPos,
                                       const glm::mat4& projection,
                                       const glm::mat4& view) const {
    glm::vec4 clipPos = projection * view * glm::vec4(worldPos, 1.0f);
    glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
    glm::vec2 screenPos = (glm::vec2(ndc) + 1.0f) * 0.5f;
    return screenPos;
}

bool HiZBuffer::isVisible(const glm::vec3& aabbMin, const glm::vec3& aabbMax,
                          const glm::mat4& projection, const glm::mat4& view) const {
    if (pyramid_.empty()) return true;

    // Project AABB corners to screen space
    glm::vec3 corners[8] = {
        glm::vec3(aabbMin.x, aabbMin.y, aabbMin.z),
        glm::vec3(aabbMax.x, aabbMin.y, aabbMin.z),
        glm::vec3(aabbMin.x, aabbMax.y, aabbMin.z),
        glm::vec3(aabbMax.x, aabbMax.y, aabbMin.z),
        glm::vec3(aabbMin.x, aabbMin.y, aabbMax.z),
        glm::vec3(aabbMax.x, aabbMin.y, aabbMax.z),
        glm::vec3(aabbMin.x, aabbMax.y, aabbMax.z),
        glm::vec3(aabbMax.x, aabbMax.y, aabbMax.z)
    };

    float minScreenX = 1.0f, maxScreenX = 0.0f;
    float minScreenY = 1.0f, maxScreenY = 0.0f;
    float minDepth = 0.0f, maxDepth = 0.0f;

    // Compute screen-space AABB and depth range
    for (int i = 0; i < 8; ++i) {
        glm::vec2 screenPos = projectToScreen_(corners[i], projection, view);
        minScreenX = std::min(minScreenX, screenPos.x);
        maxScreenX = std::max(maxScreenX, screenPos.x);
        minScreenY = std::min(minScreenY, screenPos.y);
        maxScreenY = std::max(maxScreenY, screenPos.y);

        // Simplified: use closest corner's depth
        if (i == 0) {
            glm::vec4 clipPos = projection * view * glm::vec4(corners[i], 1.0f);
            minDepth = maxDepth = clipPos.z / clipPos.w;
        }
    }

    // Clamp to [0, 1]
    minScreenX = std::max(0.0f, std::min(1.0f, minScreenX));
    maxScreenX = std::max(0.0f, std::min(1.0f, maxScreenX));
    minScreenY = std::max(0.0f, std::min(1.0f, minScreenY));
    maxScreenY = std::max(0.0f, std::min(1.0f, maxScreenY));

    // Test against coarsest level first (fastest)
    const Level& coarseLevel = pyramid_.back();
    uint32_t coarseX = static_cast<uint32_t>(minScreenX * coarseLevel.width);
    uint32_t coarseY = static_cast<uint32_t>(minScreenY * coarseLevel.height);

    if (coarseX < coarseLevel.width && coarseY < coarseLevel.height) {
        float sampledDepth = coarseLevel.depths[coarseY * coarseLevel.width + coarseX];
        // Conservative: if sampled depth is far behind, assume visible (not occluded)
        // In real implementation, would do more sophisticated test
        return minDepth <= sampledDepth + 0.01f;
    }

    return true;  // Out of bounds = assume visible
}

} // namespace RenderLib
