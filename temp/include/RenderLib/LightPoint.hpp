#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace RenderLib {

struct PointLight {
    glm::vec3 position{0.0f};
    glm::vec3 color{1.0f};
    float intensity{1.0f};
    float radius{10.0f};

    // shadow: cubemap
    GLuint depthCubeFBO{0};
    GLuint depthCubeMap{0};
    int shadowResolution{1024};

    void initShadowCube();
    void resizeShadow(int resolution);
};

} // namespace RenderLib
