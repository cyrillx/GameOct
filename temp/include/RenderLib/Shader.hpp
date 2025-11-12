#pragma once
#include <string>
#include <glad/glad.h>

namespace RenderLib {

class Shader {
public:
    Shader() = default;
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader();

    bool compileFromFiles(const std::string &vertPath, const std::string &fragPath);
    void use() const;
    GLuint id() const { return program_; }

    // uniform helpers
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const float* mat) const;
    void setVec3(const std::string &name, float x, float y, float z) const;

private:
    GLuint program_{0};
    GLuint compileShader(GLenum type, const std::string &src);
    std::string loadFile(const std::string &path);
};

} // namespace RenderLib
