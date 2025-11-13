#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>

namespace RenderLib {

class Shader;

class SSIL {
public:
    struct Config {
        int sampleCount = 16;
        float sampleRadius = 1.0f; // world units
        float falloff = 1.0f;
        bool useBentNormals = false;
        bool useHiZ = true; // use HiZ if available
    };

    SSIL();
    ~SSIL();

    bool init(int width, int height);
    void resize(int width, int height);
    void destroy();

    // G-buffer inputs: color, normal, depth, position (position optional)
    void render(GLuint colorTex, GLuint normalTex, GLuint depthTex, GLuint positionTex,
                const glm::mat4& proj, const glm::mat4& view);

    GLuint getIndirectTexture() const { return indirectTex_; }

    Config config_;

private:
    int width_ = 0;
    int height_ = 0;
    GLuint ssilFBO_ = 0;
    GLuint indirectTex_ = 0;

    std::shared_ptr<Shader> ssilShader_;
};

} // namespace RenderLib
