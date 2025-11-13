#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {

class Shader;

/**
 * @brief Edge Detection for Outline Rendering
 * 
 * Detects edges in the scene using various filter methods.
 * Can be used for:
 * - Outlines / contour rendering
 * - Cartoon rendering
 * - Technical visualization
 */
class EdgeDetection {
public:
    enum class Method {
        Sobel,          // Standard Sobel filter (good balance)
        Roberts,        // Roberts cross operator (lighter)
        Laplacian,      // Laplacian operator (strong)
        Prewitt,        // Prewitt filter (similar to Sobel)
        CannySobel      // Canny edge detection with Sobel
    };

    struct Config {
        Method method = Method::Sobel;
        float edgeThreshold = 0.1f;     // Edge detection threshold
        float edgeStrength = 1.0f;      // Detected edge strength
        glm::vec3 edgeColor = glm::vec3(0.0f);  // Edge color
        bool useNormalEdges = true;     // Use normal discontinuities
        bool useDepthEdges = true;      // Use depth discontinuities
        float depthSensitivity = 1.0f;  // How sensitive to depth differences
        float normalSensitivity = 1.0f; // How sensitive to normal differences
        bool invertEdges = false;       // Invert edge detection result
    };

    EdgeDetection();
    ~EdgeDetection();

    bool init(int width, int height);
    void resize(int width, int height);
    void destroy();

    /**
     * Detect edges from depth and normal buffers
     * @param depthTex - Scene depth texture
     * @param normalTex - Scene normals (from G-buffer or render)
     */
    void detectEdges(GLuint depthTex, GLuint normalTex = 0);

    /**
     * Get edge detection result
     */
    GLuint getEdgeTexture() const { return edgeTex_; }

    /**
     * Composite edges with original color
     * @param colorTex - Input color to composite with edges
     * @param blendAmount - How much edges to show [0, 1]
     */
    void compositeWithColor(GLuint colorTex, float blendAmount = 0.5f);

    /**
     * Get composited output
     */
    GLuint getCompositeOutput() const { return compositeTex_; }

    /**
     * Configure edge detection
     */
    void setConfig(const Config& config) { config_ = config; }
    Config getConfig() const { return config_; }

private:
    void createFramebuffers_();

    GLuint edgeFBO_ = 0;
    GLuint edgeTex_ = 0;

    GLuint compositeFBO_ = 0;
    GLuint compositeTex_ = 0;

    std::shared_ptr<Shader> edgeShader_;
    std::shared_ptr<Shader> compositeShader_;

    Config config_;
    int width_ = 0;
    int height_ = 0;
    GLuint quadVAO_ = 0;
};

} // namespace RenderLib
