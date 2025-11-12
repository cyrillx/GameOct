#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace RenderLib {

struct DirectionalLight {
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
    glm::vec3 color{1.0f};
    float intensity{1.0f};

    // shadow map
    GLuint depthMapFBO{0};
    GLuint depthMap{0};
    int shadowResolution{2048};

    void initShadowMap();
    void resizeShadow(int resolution);
};

} // namespace RenderLib
