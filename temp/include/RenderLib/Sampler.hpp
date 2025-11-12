#pragma once
#include <glad/glad.h>

namespace RenderLib {

class SamplerObject {
public:
    SamplerObject();
    ~SamplerObject();
    void setFiltering(GLenum minf, GLenum magf);
    void setWrap(GLenum s, GLenum t, GLenum r = GL_CLAMP_TO_EDGE);
    void bind(unsigned unit) const;
private:
    GLuint sampler_{0};
};

} // namespace RenderLib
