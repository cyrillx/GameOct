#pragma once
#include <glad/glad.h>
#include <string>

namespace RenderLib {

class UBO {
public:
    UBO() = default;
    ~UBO();

    // create UBO with given binding point and size (bytes)
    bool create(const std::string &name, GLuint bindingPoint, size_t size);
    void update(size_t offset, size_t size, const void* data);
    GLuint id() const { return ubo_; }

private:
    GLuint ubo_{0};
    GLuint binding_{0};
    size_t size_{0};
};

} // namespace RenderLib
