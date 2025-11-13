#include "RenderLib/Texture.hpp"
#include <iostream>

namespace RenderLib {

Texture2D::Texture2D() { glGenTextures(1, &tex_); }
Texture2D::~Texture2D() { if(tex_) glDeleteTextures(1, &tex_); }

void Texture2D::createEmpty(int w, int h, GLenum internal, GLenum format, GLenum type) {
    width_ = w; height_ = h;
    glBindTexture(GL_TEXTURE_2D, tex_);
    glTexImage2D(GL_TEXTURE_2D, 0, internal, w, h, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::fromMemory(int w, int h, GLenum format, const void* data) {
    width_ = w; height_ = h;
    glBindTexture(GL_TEXTURE_2D, tex_);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind(unsigned unit) const { glActiveTexture(GL_TEXTURE0 + unit); glBindTexture(GL_TEXTURE_2D, tex_); }

} // namespace RenderLib
