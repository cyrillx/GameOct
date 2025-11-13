#include "RenderLib/DecalSystem.hpp"
#include "RenderLib/Shader.hpp"
#include "RenderLib/Texture2D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>

namespace RenderLib {

DecalSystem::DecalSystem() {}

DecalSystem::~DecalSystem() {
    destroy();
}

bool DecalSystem::init(const Config& config) {
    config_ = config;

    // Create decal shader
    decalShader_ = new Shader();
    if (!decalShader_->compile("decal.vert", "decal.frag")) {
        std::cerr << "Failed to compile decal shaders\n";
        delete decalShader_;
        decalShader_ = nullptr;
        return false;
    }

    // Create cube mesh for inverse hull
    createCubeMesh_();

    return true;
}

void DecalSystem::destroy() {
    if (cubeVAO_) {
        glDeleteVertexArrays(1, &cubeVAO_);
        cubeVAO_ = 0;
    }
    if (cubeVBO_) {
        glDeleteBuffers(1, &cubeVBO_);
        cubeVBO_ = 0;
    }
    if (cubeEBO_) {
        glDeleteBuffers(1, &cubeEBO_);
        cubeEBO_ = 0;
    }
    if (decalShader_) {
        delete decalShader_;
        decalShader_ = nullptr;
    }
    decals_.clear();
}

void DecalSystem::createCubeMesh_() {
    // Cube vertices
    float vertices[] = {
        // Front face
        -1, -1,  1,  // 0
         1, -1,  1,  // 1
         1,  1,  1,  // 2
        -1,  1,  1,  // 3
        // Back face
        -1, -1, -1,  // 4
         1, -1, -1,  // 5
         1,  1, -1,  // 6
        -1,  1, -1,  // 7
    };

    // Indices (back faces for inverted rendering)
    uint32_t indices[] = {
        // Front face (inverted)
        0, 2, 1, 0, 3, 2,
        // Back face (inverted)
        4, 5, 6, 4, 6, 7,
        // Left face (inverted)
        4, 0, 1, 4, 1, 5,
        // Right face (inverted)
        7, 6, 2, 7, 2, 3,
        // Top face (inverted)
        7, 3, 2, 7, 2, 6,
        // Bottom face (inverted)
        4, 1, 0, 4, 5, 1,
    };
    cubeIndexCount_ = 36;

    glGenVertexArrays(1, &cubeVAO_);
    glGenBuffers(1, &cubeVBO_);
    glGenBuffers(1, &cubeEBO_);

    glBindVertexArray(cubeVAO_);
    
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void DecalSystem::addDecal(const glm::vec3& position, const glm::vec3& normal,
                          std::shared_ptr<Texture2D> albedoTex,
                          std::shared_ptr<Texture2D> normalTex,
                          const glm::vec3& scale,
                          float lifetime) {
    if (decals_.size() >= config_.maxDecals) {
        return;  // Queue is full
    }

    // Create rotation matrix from normal
    glm::vec3 up = glm::abs(normal.y) < 0.9f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
    glm::vec3 right = glm::normalize(glm::cross(normal, up));
    up = glm::normalize(glm::cross(right, normal));

    glm::mat4 matrix = glm::mat4(1.0f);
    matrix[0] = glm::vec4(right, 0.0f);
    matrix[1] = glm::vec4(up, 0.0f);
    matrix[2] = glm::vec4(normal, 0.0f);
    matrix[3] = glm::vec4(position, 1.0f);
    matrix = glm::scale(matrix, scale);

    Decal decal;
    decal.worldMatrix = matrix;
    decal.albedo = albedoTex;
    decal.normal = normalTex;
    decal.lifetime = lifetime;
    decal.fadeStart = lifetime * 0.8f;  // Start fading at 80% lifetime

    decals_.push_back(decal);
}

void DecalSystem::update(float deltaTime) {
    for (auto& decal : decals_) {
        decal.elapsedTime += deltaTime;
        decal.opacity = decal.getAlpha();
    }

    // Remove dead decals
    decals_.erase(
        std::remove_if(decals_.begin(), decals_.end(),
                      [](const Decal& d) { return !d.isAlive(); }),
        decals_.end()
    );
}

void DecalSystem::render(const glm::mat4& projection, const glm::mat4& view) {
    if (decals_.empty() || !decalShader_) return;

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    // First pass: fill stencil buffer with cube volumes
    glBindVertexArray(cubeVAO_);
    for (const auto& decal : decals_) {
        glm::mat4 model = view * decal.worldMatrix;
        // Stencil pass
    }

    // Second pass: render decals using stencil
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    decalShader_->use();
    glBindVertexArray(cubeVAO_);

    for (const auto& decal : decals_) {
        glm::mat4 model = decal.worldMatrix;
        glm::mat4 modelView = view * model;
        glm::mat4 mvp = projection * modelView;

        decalShader_->setMat4("mvp", mvp);
        decalShader_->setMat4("modelView", modelView);
        decalShader_->setFloat("opacity", decal.opacity);

        if (decal.albedo) {
            decalShader_->setInt("albedoTex", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, decal.albedo->textureID);
        }

        if (decal.normal) {
            decalShader_->setInt("normalTex", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, decal.normal->textureID);
        }

        glDrawElements(GL_TRIANGLES, cubeIndexCount_, GL_UNSIGNED_INT, 0);
    }

    glDisable(GL_STENCIL_TEST);
    glBindVertexArray(0);
}

}  // namespace RenderLib
