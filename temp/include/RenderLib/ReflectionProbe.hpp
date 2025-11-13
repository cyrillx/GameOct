#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

namespace RenderLib {

class Renderer; // forward

// Simple reflection probe which holds a cubemap and helper to bake it.
class ReflectionProbe {
public:
    ReflectionProbe();
    ~ReflectionProbe();

    // Create cubemap texture of given resolution (per-face)
    bool init(int resolution = 128);
    void destroy();

    // Bake helper: user should provide a callback that renders scene for a given view matrix.
    // The callback signature: void renderFace(const glm::mat4& view, const glm::mat4& proj, unsigned faceIndex)
    bool bakeAt(const glm::vec3& position, std::function<void(const glm::mat4&, const glm::mat4&, unsigned)> renderFace);

    GLuint getCubemap() const { return cubemapTex_; }
    int getResolution() const { return resolution_; }

private:
    GLuint cubemapTex_ = 0;
    GLuint fbo_ = 0;
    int resolution_ = 128;
};

} // namespace RenderLib
