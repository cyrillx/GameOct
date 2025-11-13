#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

namespace RenderLib {

// Simple irradiance volume storing irradiance probes on a regular 3D grid.
// This class provides storage and sampling helpers. Baking is left to the user
// (via ReflectionProbe::bakeAt or custom render-to-cubemap + convolution).
class IrradianceVolume {
public:
    struct Config {
        glm::vec3 minBounds = glm::vec3(-50.0f);
        glm::vec3 maxBounds = glm::vec3(50.0f);
        glm::ivec3 probeCounts = glm::ivec3(8, 4, 8);
    };

    IrradianceVolume();
    ~IrradianceVolume();

    bool init(const Config& cfg);
    void destroy();

    // Set irradiance for probe at grid index (x,y,z)
    void setProbeIrradiance(int x, int y, int z, const glm::vec3& rgb);

    // Upload CPU buffer into GPU 3D texture
    void uploadToGPU();

    // Bind the 3D irradiance texture to a texture unit
    void bind(int unit) const;

    GLuint getTexture() const { return irradianceTex_; }

    Config config_;

private:
    int totalProbes_ = 0;
    std::vector<float> cpuData_; // RGB32f per probe
    GLuint irradianceTex_ = 0;
};

} // namespace RenderLib
