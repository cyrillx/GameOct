#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

namespace RenderLib {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 tangent;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    // create from arrays (copies data)
    void create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void destroy();

    void draw() const;

    GLuint vao() const { return vao_; }
    GLsizei indexCount() const { return indexCount_; }

private:
    GLuint vao_{0}, vbo_{0}, ebo_{0};
    GLsizei indexCount_{0};
};

} // namespace RenderLib
