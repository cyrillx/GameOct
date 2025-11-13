#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace RenderLib {

class Mesh;
class Material;
class Frustum;
class RenderQueue;
class InstanceBuffer;
class Terrain;
class Water;
class HiZBuffer;
class LODGroup;
class Vegetation;
class MirrorWater;
class VolumetricLighting;
class DecalSystem;
class ShadowScheduler;
class WindSystem;
class TemporalSSAO;
class DynamicResolution;
class TemporalFilter;
class ImposterGenerator;

class ForwardRenderer {
public:
    ForwardRenderer();
    ~ForwardRenderer();

    bool init(int width, int height);
    void resize(int w, int h);

    void beginFrame();
    void endFrame();

    // Standard rendering with optional transparency
    void submitMesh(const Mesh& mesh, const Material& mat, const glm::mat4& model, bool transparent = false);
    
    // Instanced rendering
    void submitInstanced(const Mesh& mesh, const Material& mat, 
                        const std::vector<glm::mat4>& transforms);

    // Frustum culling
    void enableFrustumCulling(bool enable) { frustumCullingEnabled_ = enable; }
    void updateFrustum(const glm::mat4& viewProj);
    bool isMeshInFrustum(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const;

    // Hi-Z occlusion culling
    void updateHiZ(const float* depthBuffer);
    bool isOccluded(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const;

    // Terrain and water rendering
    void renderTerrain(Terrain* terrain, const glm::mat4& projection, const glm::mat4& view);
    void renderWater(Water* water, const glm::mat4& projection, const glm::mat4& view);
    void renderVegetation(Vegetation* vegetation, const glm::mat4& projection, const glm::mat4& view);
    void renderMirrorWater(MirrorWater* water, const glm::mat4& projection, const glm::mat4& view);
    void renderMirrorWaterReflection(MirrorWater* water, const glm::mat4& projection, const glm::mat4& view);

    // Phase 6 Systems
    // Volumetric lighting
    void renderVolumetricLighting(VolumetricLighting* volumetric, const glm::mat4& projection, const glm::mat4& view);
    
    // Decals
    void renderDecals(DecalSystem* decals, const glm::mat4& projection, const glm::mat4& view);
    
    // Shadow scheduling
    void updateShadowSchedule(ShadowScheduler* scheduler);
    
    // Wind system
    WindSystem* getWindSystem() { return windSystem_.get(); }
    void setWindSystem(std::shared_ptr<WindSystem> wind) { windSystem_ = wind; }
    
    // Temporal SSAO
    void renderTemporalSSAO(TemporalSSAO* ssao);
    
    // Dynamic resolution
    void setDynamicResolution(std::shared_ptr<DynamicResolution> dynRes) { dynamicResolution_ = dynRes; }
    void applyDynamicResolution(GLuint lowResTexture);
    
    // Temporal filtering
    void applyTemporalFilter(GLuint currentFrame, GLuint velocityBuffer = 0);
    
    // Soft shadows control
    void enableSoftShadows(bool enable) { softShadowsEnabled_ = enable; }
    void setSoftShadowParams(float penumbraSize, int samples);

    int width() const { return width_; }
    int height() const { return height_; }
    GLuint hdrColorBuffer() const { return colorBuffer_.id(); }
    
    // Access to internal systems
    RenderQueue* getRenderQueue() { return renderQueue_.get(); }
    Frustum* getFrustum() { return frustum_.get(); }

private:
    int width_{0}, height_{0};
    GLuint hdrFBO_{0};
    class Texture2D colorBuffer_;
    GLuint lightsUBO_{0};
    
    std::unique_ptr<RenderQueue> renderQueue_;
    std::unique_ptr<Frustum> frustum_;
    std::unique_ptr<InstanceBuffer> instanceBuffer_;
    std::unique_ptr<HiZBuffer> hizBuffer_;
    
    std::shared_ptr<VolumetricLighting> volumetricLighting_;
    std::shared_ptr<DecalSystem> decalSystem_;
    std::shared_ptr<ShadowScheduler> shadowScheduler_;
    std::shared_ptr<WindSystem> windSystem_;
    std::shared_ptr<TemporalSSAO> temporalSSAO_;
    std::shared_ptr<DynamicResolution> dynamicResolution_;
    std::shared_ptr<TemporalFilter> temporalFilter_;
    
    bool frustumCullingEnabled_{false};
    bool occlusionCullingEnabled_{false};
    bool softShadowsEnabled_{false};
    
    float softShadowPenumbraSize_{2.0f};
    int softShadowSamples_{16};
    
    void renderQueue();
};

struct Material {
    class Texture2D* diffuse{nullptr};
    class Texture2D* specular{nullptr};
    class Texture2D* normal{nullptr};
    float shininess{32.0f};
};

struct Mesh {
    class Mesh* mesh_ptr;
};

} // namespace RenderLib
