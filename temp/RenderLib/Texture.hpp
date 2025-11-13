#pragma once
#include <glad/glad.h>
#include <string>

namespace RenderLib {

class Texture2D {
public:
    Texture2D();
    ~Texture2D();

    void createEmpty(int w, int h, GLenum internal = GL_RGBA16F, GLenum format = GL_RGBA, GLenum type = GL_FLOAT);
    void fromMemory(int w, int h, GLenum format, const void* data);
    void bind(unsigned unit) const;
    GLuint id() const { return tex_; }

private:
    GLuint tex_{0};
    int width_{0}, height_{0};
};

} // namespace RenderLib
