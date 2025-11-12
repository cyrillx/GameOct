#include "RenderLib/UBO.hpp"
#include <iostream>

namespace RenderLib {

UBO::~UBO() {
    if(ubo_) glDeleteBuffers(1, &ubo_);
}

bool UBO::create(const std::string &name, GLuint bindingPoint, size_t size) {
    if(size == 0) return false;
    binding_ = bindingPoint;
    size_ = size;
    glGenBuffers(1, &ubo_);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    glBufferData(GL_UNIFORM_BUFFER, size_, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_, ubo_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return true;
}

void UBO::update(size_t offset, size_t size, const void* data) {
    if(!ubo_ || offset + size > size_) return;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

} // namespace RenderLib
