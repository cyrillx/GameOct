#include "RenderLib/Sampler.hpp"
#include <iostream>

namespace RenderLib {

SamplerObject::SamplerObject() {
    glGenSamplers(1, &sampler_);
    glSamplerParameteri(sampler_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

SamplerObject::~SamplerObject() {
    if(sampler_) glDeleteSamplers(1, &sampler_);
}

void SamplerObject::setFiltering(GLenum minf, GLenum magf) {
    glSamplerParameteri(sampler_, GL_TEXTURE_MIN_FILTER, minf);
    glSamplerParameteri(sampler_, GL_TEXTURE_MAG_FILTER, magf);
}

void SamplerObject::setWrap(GLenum s, GLenum t, GLenum r) {
    glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_S, s);
    glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_T, t);
    glSamplerParameteri(sampler_, GL_TEXTURE_WRAP_R, r);
}

void SamplerObject::bind(unsigned unit) const {
    glBindSampler(unit, sampler_);
}

} // namespace RenderLib
