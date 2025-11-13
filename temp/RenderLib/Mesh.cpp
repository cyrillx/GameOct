#include "RenderLib/Mesh.hpp"
#include <cstring>

namespace RenderLib {

Mesh::Mesh() {}
Mesh::~Mesh() { destroy(); }

void Mesh::create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    destroy();
    indexCount_ = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // layout location mapping must match shaders
    // pos (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    // normal (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // texcoord (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    // tangent (location = 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glBindVertexArray(0);
}

void Mesh::destroy() {
    if(ebo_) { glDeleteBuffers(1, &ebo_); ebo_ = 0; }
    if(vbo_) { glDeleteBuffers(1, &vbo_); vbo_ = 0; }
    if(vao_) { glDeleteVertexArrays(1, &vao_); vao_ = 0; }
    indexCount_ = 0;
}

void Mesh::draw() const {
    if(!vao_ || indexCount_ == 0) return;
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

} // namespace RenderLib
