#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace RenderLib {

struct Mesh;
struct Material;

class ForwardRenderer {
public:
    ForwardRenderer();
    ~ForwardRenderer();

    bool init(int width, int height);
    void resize(int w, int h);

    void beginFrame();
    void endFrame();

    void submitMesh(const Mesh& mesh, const Material& mat, const glm::mat4& model);

    int width() const { return width_; }
    int height() const { return height_; }
    GLuint hdrColorBuffer() const { return colorBuffer_.id(); }

private:
    int width_{0}, height_{0};
    GLuint hdrFBO_{0};
    class Texture2D colorBuffer_;
    GLuint lightsUBO_{0};
};

struct Mesh;
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
