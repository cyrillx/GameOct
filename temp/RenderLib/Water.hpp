#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"

namespace RenderLib {

/**
 * @brief Water plane with reflections, normal mapping, and depth-based coloring
 */
class Water {
public:
    struct Config {
        float width = 512.0f;
        float height = 512.0f;
        float tileSize = 1.0f;
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 shallowColor = glm::vec3(0.1f, 0.6f, 0.8f);
        glm::vec3 deepColor = glm::vec3(0.0f, 0.1f, 0.3f);
        float depthScale = 50.0f;  // Distance until water is fully deep color
        float normalScale = 0.2f;
        float fresnelPower = 2.0f;
    };

    Water();
    ~Water();

    /**
     * @brief Initialize water plane with config
     */
    bool init(const Config& config);

    /**
     * @brief Set normal map texture (will be scrolled)
     */
    void setNormalTexture(std::shared_ptr<Texture2D> tex) { normalTex_ = tex; }

    /**
     * @brief Set reflection texture (rendered scene reflection)
     */
    void setReflectionTexture(std::shared_ptr<Texture2D> tex) { reflectionTex_ = tex; }

    /**
     * @brief Set depth texture from scene (for depth-based coloring)
     */
    void setDepthTexture(std::shared_ptr<Texture2D> tex) { depthTex_ = tex; }

    /**
     * @brief Update wave scrolling (call once per frame)
     */
    void updateWaves(float deltaTime, float speed = 0.1f);

    /**
     * @brief Render water plane
     */
    void render(const glm::mat4& projection, const glm::mat4& view);

    /**
     * @brief Get mesh for picking or collision
     */
    Mesh* getMesh() { return mesh_.get(); }

    const Config& getConfig() const { return config_; }
    glm::vec2 getNormalOffset() const { return normalOffset_; }

private:
    Config config_;
    std::unique_ptr<Mesh> mesh_;

    std::shared_ptr<Texture2D> normalTex_;
    std::shared_ptr<Texture2D> reflectionTex_;
    std::shared_ptr<Texture2D> depthTex_;

    glm::vec2 normalOffset_;  // Scrolling offset for normal maps
};

} // namespace RenderLib
