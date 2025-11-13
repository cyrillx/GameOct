#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Instancing.hpp"

namespace RenderLib {

/**
 * @brief Vegetation system for grass and trees with LOD and wind animation
 * 
 * Grass: Alpha-tested planes with wind deformation
 * Trees: Full mesh close, billboards far
 */
class Vegetation {
public:
    enum class Type {
        Grass,
        Tree,
        Bush
    };

    struct GrassConfig {
        uint32_t grassCount = 1000;        // Number of grass patches per area
        float grassWidth = 0.5f;
        float grassHeight = 1.5f;
        float grassDensity = 0.2f;         // Coverage [0, 1]
        glm::vec2 areaSize = {100.0f, 100.0f};  // World space dimensions
    };

    struct TreeConfig {
        uint32_t treeCount = 100;
        float trunkRadius = 0.3f;
        float treeHeight = 10.0f;
        glm::vec2 areaSize = {200.0f, 200.0f};
    };

    Vegetation();
    ~Vegetation();

    /**
     * @brief Initialize grass system
     */
    bool initGrass(const GrassConfig& config, std::shared_ptr<Texture2D> grassTexture);

    /**
     * @brief Initialize tree system
     */
    bool initTrees(const TreeConfig& config,
                   std::shared_ptr<Mesh> treeMesh,
                   std::shared_ptr<Mesh> billboardMesh);

    /**
     * @brief Update vegetation (wind animation, LOD)
     */
    void update(float deltaTime);

    /**
     * @brief Render all vegetation
     */
    void render() const;

    /**
     * @brief Set wind parameters
     */
    void setWind(float strength = 0.5f, float frequency = 1.0f) {
        windStrength_ = strength;
        windFrequency_ = frequency;
    }

    /**
     * @brief Get current wind phase (for shader)
     */
    float getWindPhase() const { return windPhase_; }

    /**
     * @brief Get grass instance buffer
     */
    InstanceBuffer* getGrassBuffer() { return grassBuffer_.get(); }

    /**
     * @brief Get tree instance buffer
     */
    InstanceBuffer* getTreeBuffer() { return treeBuffer_.get(); }

private:
    GrassConfig grassConfig_;
    TreeConfig treeConfig_;

    std::unique_ptr<Mesh> grassMesh_;        // Single plane (2 triangles)
    std::unique_ptr<Mesh> treeMesh_;         // Full tree
    std::unique_ptr<Mesh> treeBillboard_;    // Billboard quad

    std::shared_ptr<Texture2D> grassTexture_;
    std::shared_ptr<Texture2D> treeTexture_;
    std::shared_ptr<Texture2D> treeBillboardTexture_;

    std::unique_ptr<InstanceBuffer> grassBuffer_;
    std::unique_ptr<InstanceBuffer> treeBuffer_;

    float windPhase_ = 0.0f;
    float windStrength_ = 0.5f;
    float windFrequency_ = 1.0f;

    void createGrassMesh_();
    void createTreeBillboard_();
    void generateGrassInstances_();
    void generateTreeInstances_();
};

} // namespace RenderLib
