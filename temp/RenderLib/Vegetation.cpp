#include "RenderLib/Vegetation.hpp"
#include <random>

namespace RenderLib {

Vegetation::Vegetation() = default;
Vegetation::~Vegetation() = default;

bool Vegetation::initGrass(const GrassConfig& config, std::shared_ptr<Texture2D> grassTexture) {
    grassConfig_ = config;
    grassTexture_ = grassTexture;

    createGrassMesh_();
    generateGrassInstances_();

    grassBuffer_ = std::make_unique<InstanceBuffer>();
    if (!grassBuffer_->create(config.grassCount)) {
        return false;
    }

    return true;
}

bool Vegetation::initTrees(const TreeConfig& config,
                          std::shared_ptr<Mesh> treeMesh,
                          std::shared_ptr<Mesh> billboardMesh) {
    treeConfig_ = config;
    treeMesh_ = std::move(treeMesh);
    treeBillboard_ = std::move(billboardMesh);

    createTreeBillboard_();
    generateTreeInstances_();

    treeBuffer_ = std::make_unique<InstanceBuffer>();
    if (!treeBuffer_->create(config.treeCount)) {
        return false;
    }

    return true;
}

void Vegetation::createGrassMesh_() {
    grassMesh_ = std::make_unique<Mesh>();

    // Single grass blade plane (2 triangles)
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;

    float w = grassConfig_.grassWidth / 2.0f;
    float h = grassConfig_.grassHeight;

    // Two planes crossed for fullness (X and Z direction)
    // Plane 1: X-axis
    vertices.push_back({glm::vec3(-w, 0, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(w, 0, 0), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(w, h, 0), glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(-w, h, 0), glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(1, 0, 0)});

    // Plane 2: Z-axis
    vertices.push_back({glm::vec3(0, 0, -w), glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(0, 0, w), glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(0, h, w), glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(0, h, -w), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(1, 0, 0)});

    // Indices for both planes
    indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7};

    grassMesh_->create(vertices, indices);
}

void Vegetation::createTreeBillboard_() {
    if (!treeBillboard_) {
        treeBillboard_ = std::make_unique<Mesh>();

        std::vector<Mesh::Vertex> vertices;
        std::vector<uint32_t> indices;

        float w = 5.0f;  // Billboard width
        float h = 10.0f; // Billboard height

        vertices.push_back({glm::vec3(-w, 0, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0)});
        vertices.push_back({glm::vec3(w, 0, 0), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(1, 0, 0)});
        vertices.push_back({glm::vec3(w, h, 0), glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(1, 0, 0)});
        vertices.push_back({glm::vec3(-w, h, 0), glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(1, 0, 0)});

        indices = {0, 1, 2, 0, 2, 3};

        treeBillboard_->create(vertices, indices);
    }
}

void Vegetation::generateGrassInstances_() {
    std::mt19937 rng(12345);  // Fixed seed for reproducibility
    std::uniform_real_distribution<float> posX(-grassConfig_.areaSize.x / 2, grassConfig_.areaSize.x / 2);
    std::uniform_real_distribution<float> posZ(-grassConfig_.areaSize.y / 2, grassConfig_.areaSize.y / 2);
    std::uniform_real_distribution<float> rotation(0, 6.28318f);
    std::uniform_real_distribution<float> scale(0.8f, 1.2f);

    std::vector<InstanceData> instances;

    for (uint32_t i = 0; i < grassConfig_.grassCount; ++i) {
        glm::vec3 pos(posX(rng), 0, posZ(rng));
        float rot = rotation(rng);
        float scl = scale(rng);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model, rot, glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(scl, scl, scl));

        instances.push_back({model, glm::vec4(0.8f, 0.9f, 0.7f, 1.0f)});  // Greenish color
    }

    if (grassBuffer_) {
        grassBuffer_->updateData(instances);
    }
}

void Vegetation::generateTreeInstances_() {
    std::mt19937 rng(54321);
    std::uniform_real_distribution<float> posX(-treeConfig_.areaSize.x / 2, treeConfig_.areaSize.x / 2);
    std::uniform_real_distribution<float> posZ(-treeConfig_.areaSize.y / 2, treeConfig_.areaSize.y / 2);
    std::uniform_real_distribution<float> scale(0.8f, 1.3f);

    std::vector<InstanceData> instances;

    for (uint32_t i = 0; i < treeConfig_.treeCount; ++i) {
        glm::vec3 pos(posX(rng), 0, posZ(rng));
        float scl = scale(rng);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, glm::vec3(scl, scl, scl));

        instances.push_back({model, glm::vec4(0.6f, 0.5f, 0.3f, 1.0f)});  // Brown color
    }

    if (treeBuffer_) {
        treeBuffer_->updateData(instances);
    }
}

void Vegetation::update(float deltaTime) {
    windPhase_ += deltaTime * windFrequency_;
    if (windPhase_ > 6.28318f) windPhase_ -= 6.28318f;
}

void Vegetation::render() const {
    // Render grass blades
    if (grassBuffer_ && grassMesh_) {
        grassBuffer_->bindSSBO(3);
        grassMesh_->draw();
    }

    // Render trees (either full mesh or billboards depending on distance)
    if (treeBuffer_ && treeMesh_) {
        treeBuffer_->bindSSBO(3);
        treeMesh_->draw();
    }
}

} // namespace RenderLib
