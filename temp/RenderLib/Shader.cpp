#include "RenderLib/Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace RenderLib {

std::string Shader::loadFile(const std::string &path) {
    std::ifstream ifs(path);
    if(!ifs) return {};
    std::stringstream ss; ss << ifs.rdbuf();
    return ss.str();
}

GLuint Shader::compileShader(GLenum type, const std::string &src) {
    GLuint s = glCreateShader(type);
    const char* cstr = src.c_str();
    glShaderSource(s, 1, &cstr, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok) {
        char buf[1024]; glGetShaderInfoLog(s, 1024, nullptr, buf);
        std::cerr << "Shader compile error: " << buf << '\n';
        glDeleteShader(s);
        return 0;
    }
    return s;
}

bool Shader::compileFromFiles(const std::string &vertPath, const std::string &fragPath) {
    auto vsrc = loadFile(vertPath);
    auto fsrc = loadFile(fragPath);
    if(vsrc.empty() || fsrc.empty()) return false;
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsrc);
    if(!vs || !fs) return false;
    program_ = glCreateProgram();
    glAttachShader(program_, vs);
    glAttachShader(program_, fs);
    glLinkProgram(program_);
    GLint ok; glGetProgramiv(program_, GL_LINK_STATUS, &ok);
    if(!ok) {
        char buf[1024]; glGetProgramInfoLog(program_, 1024, nullptr, buf);
        std::cerr << "Program link error: " << buf << '\n';
        glDeleteProgram(program_);
        program_ = 0;
    }
    glDeleteShader(vs); glDeleteShader(fs);
    return program_ != 0;
}

Shader::~Shader() { if(program_) glDeleteProgram(program_); }

void Shader::use() const { if(program_) glUseProgram(program_); }

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
}
void Shader::setMat4(const std::string &name, const float* mat) const {
    glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, mat);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(program_, name.c_str()), x, y, z);
}

} // namespace RenderLib
