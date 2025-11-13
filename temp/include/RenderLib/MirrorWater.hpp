#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Mesh.hpp"
#include "Texture.hpp"

namespace RenderLib {

class ForwardRenderer;  // Forward declaration

/**
 * @brief Mirror water with scene reflections
 * 
 * Renders scene reflected in water plane using mirror matrix.
 * Combines reflection with water surface waves for realistic effect.
 */
class MirrorWater {
public:
    struct Config {
        float width = 512.0f;
        float height = 512.0f;
        float tileSize = 1.0f;
        glm::vec3 position = glm::vec3(0, 0, 0);
        
        glm::vec3 shallowColor = glm::vec3(0.1f, 0.6f, 0.8f);
        glm::vec3 deepColor = glm::vec3(0.0f, 0.1f, 0.3f);
        float depthScale = 50.0f;
        float fresnelPower = 2.0f;
        float normalScale = 0.2f;
        
        uint32_t reflectionTexSize = 512;  // Reflection texture resolution
    };

    MirrorWater();
    ~MirrorWater();

    /**
     * @brief Initialize mirror water
     */
    bool init(const Config& config);

    /**
     * @brief Set normal map texture (will be scrolled)
     */
    void setNormalTexture(std::shared_ptr<Texture2D> tex) { normalTex_ = tex; }

    /**
     * @brief Update wave scrolling (call once per frame)
     */
    void updateWaves(float deltaTime, float speed = 0.1f);

    /**
     * @brief Render water plane with reflections
     * 
     * Calls renderReflection internally to capture scene reflection
     */
    void render(const glm::mat4& projection, const glm::mat4& view);

    /**
     * @brief Render scene reflection (mirror pass)
     * 
     * Call this BEFORE main render to capture reflection to texture.
     * Renders scene with mirrored view matrix.
     */
    void renderReflection(ForwardRenderer* renderer,
                         const glm::mat4& projection,
                         const glm::mat4& view);

    /**
     * @brief Get reflection texture (for debug display)
     */
    Texture2D* getReflectionTexture() { return reflectionTex_.get(); }

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
    std::unique_ptr<Texture2D> reflectionTex_;
    uint32_t reflectionFBO_ = 0;

    glm::vec2 normalOffset_;

    glm::mat4 getMirrorMatrix_(float waterHeight) const;
};

} // namespace RenderLib
