#include "RenderLib/GPUCuller.hpp"
#include "RenderLib/Shader.hpp"
#include <iostream>

namespace RenderLib {

GPUCuller::GPUCuller() = default;
GPUCuller::~GPUCuller() { destroy(); }

bool GPUCuller::init(int maxInstances) {
    maxInstances_ = maxInstances;

    // Create feedback buffer large enough to hold maxInstances * 4 vec4's (mat4)
    glGenBuffers(1, &feedbackVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, feedbackVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * maxInstances_, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create query object
    glGenQueries(1, &query_);

    // Load culling shader (vertex + geometry) - geometry will emit matrices
    cullShader_ = std::make_shared<Shader>("shaders/gpu_cull.vert", "shaders/gpu_cull.geom");
    if (!cullShader_) {
        std::cerr << "Failed to load GPU cull shaders\n";
        return false;
    }

    // Setup VAO for drawing from feedback (will be configured later)
    glGenVertexArrays(1, &tfVAO_);
    glBindVertexArray(tfVAO_);

    // Feedback buffer will be bound as per-instance attribute during draw
    glBindVertexArray(0);

    return true;
}

void GPUCuller::destroy() {
    if (feedbackVBO_) glDeleteBuffers(1, &feedbackVBO_);
    if (query_) glDeleteQueries(1, &query_);
    feedbackVBO_ = 0;
    query_ = 0;
    cullShader_.reset();
    if (tfVAO_) glDeleteVertexArrays(1, &tfVAO_);
    tfVAO_ = 0;
}

void GPUCuller::setInstanceBuffer(GLuint vbo, int instanceCount) {
    sourceInstanceVBO_ = vbo;
    instanceCount_ = instanceCount;
}

void GPUCuller::cull(GLuint renderVAO, const glm::mat4& viewProj) {
    if (!cullShader_) return;

    // Bind transform feedback
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbackVBO_);

    // Begin query for primitives written
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query_);

    // Enable rasterizer discard so we don't write to default framebuffer
    glEnable(GL_RASTERIZER_DISCARD);

    cullShader_->use();
    cullShader_->setMat4("uViewProj", viewProj);

    // Bind source instance buffer to attribute locations expected by shader
    glBindVertexArray(renderVAO);

    // Setup transform feedback varyings before linking shader in Shader class - assumed done
    glBeginTransformFeedback(GL_POINTS);

    // Draw the renderVAO with instancing using instanceCount_
    glDrawArraysInstanced(GL_POINTS, 0, 1, instanceCount_);

    glEndTransformFeedback();

    // Disable rasterizer discard
    glDisable(GL_RASTERIZER_DISCARD);

    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

    // Get number written
    GLuint written = 0;
    glGetQueryObjectuiv(query_, GL_QUERY_RESULT, &written);

    // Setup tfVAO to use feedbackVBO_ as per-instance mat4 attribute
    glBindVertexArray(tfVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, feedbackVBO_);
    // Matrix occupies 4 attribute slots
    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(4 + i); // location 4..7 reserved for instance matrix
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 4 * i));
        glVertexAttribDivisor(4 + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Store written count in instanceCount_
    instanceCount_ = written;
}

void GPUCuller::drawInstanced(GLuint meshVAO, GLsizei indexCount) {
    if (instanceCount_ == 0) return;

    glBindVertexArray(meshVAO);

    // Bind tfVAO attributes as per-instance data
    glBindVertexArray(tfVAO_);

    // Draw mesh with instancing, instanceCount_ instances
    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount_);

    glBindVertexArray(0);
}

} // namespace RenderLib
