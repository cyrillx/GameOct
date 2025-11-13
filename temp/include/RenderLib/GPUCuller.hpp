#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {
class Shader;

// GPU-assisted culling using transform feedback + geometry shader.
// Requires GL 3.3 (geometry shader & transform feedback supported). This
// class captures visible instance matrices into a buffer, then uses that
// buffer as an instanced attribute for drawing.
class GPUCuller {
public:
    GPUCuller();
    ~GPUCuller();

    bool init(int maxInstances = 16384);
    void destroy();

    // Provide VBO containing instance model matrices (mat4 per instance)
    void setInstanceBuffer(GLuint vbo, int instanceCount);

    // Perform culling: caller provides view/proj and a callback to render bounding geometry
    // The renderBounding callback should issue a draw call that uses the instance buffer
    // with divisor=1 (e.g., draw points or boxes). The geometry shader will emit only
    // visible instances into transform-feedback.
    void cull(GLuint renderVAO, const glm::mat4& viewProj);

    // After cull, draw using captured instances with provided mesh VAO & index count
    void drawInstanced(GLuint meshVAO, GLsizei indexCount);

private:
    int maxInstances_ = 0;
    GLuint feedbackVBO_ = 0; // will hold captured matrices
    GLuint query_ = 0;
    std::shared_ptr<Shader> cullShader_; // vertex+geom used for TF
    GLuint tfVAO_ = 0; // VAO for instanced draw using feedbackVBO_
    int instanceCount_ = 0;
    GLuint sourceInstanceVBO_ = 0;
};

} // namespace RenderLib
