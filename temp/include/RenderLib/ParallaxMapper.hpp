#pragma once
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {

class Shader;
class Texture2D;

/**
 * @brief Optional Parallax Mapping / Displacement Mapping
 * 
 * Adds depth to surfaces using height maps.
 * Supports multiple methods:
 * - Simple parallax mapping (fast)
 * - Parallax occlusion mapping (better quality)
 * - Steep parallax mapping (good balance)
 */
class ParallaxMapper {
public:
    enum class Method {
        Parallax,              // Basic parallax mapping
        Relief,                // Parallax relief mapping
        Occlusion,             // Parallax occlusion mapping (best quality)
        Steep                  // Steep parallax mapping
    };

    struct Config {
        float heightScale = 0.1f;      // Height map scale factor
        int minLayers = 8;             // Min layers for parallax
        int maxLayers = 32;            // Max layers for parallax
        bool useOcclusionMapping = true;
        Method method = Method::Occlusion;
        float depthBias = 0.0f;        // Bias for depth
    };

    ParallaxMapper();
    ~ParallaxMapper();

    /**
     * Check if parallax mapping is enabled globally
     */
    static bool isEnabled();
    static void setEnabled(bool enable);

    /**
     * Configure parallax mapping quality
     */
    void setConfig(const Config& config) { config_ = config; }
    Config getConfig() const { return config_; }

    /**
     * Enable/disable parallax for specific texture
     */
    void enableForTexture(bool enable) { enabled_ = enable; }
    bool isEnabledForTexture() const { return enabled_; }

private:
    static bool globallyEnabled_;
    Config config_;
    bool enabled_ = true;
};

} // namespace RenderLib
