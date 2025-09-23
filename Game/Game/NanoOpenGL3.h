#pragma once

//=============================================================================
// Shaders
//=============================================================================
int GetUniformLocation(GLuint program, std::string_view name);
void SetUniform(GLuint id, float s);
void SetUniform(GLuint id, int s);
void SetUniform(GLuint id, const glm::vec2& v);
void SetUniform(GLuint id, std::span<const glm::vec2> v);
void SetUniform(GLuint id, const glm::vec3& v);
void SetUniform(GLuint id, std::span<const glm::vec3> v);
void SetUniform(GLuint id, const glm::vec4& v);
void SetUniform(GLuint id, std::span<const glm::vec4> v);
void SetUniform(GLuint id, const glm::quat& v);
void SetUniform(GLuint id, const glm::mat3& m);
void SetUniform(GLuint id, const glm::mat4& m);

//=============================================================================
// VertexAttribute
//=============================================================================

struct VertexAttribute final
{
	GLenum      type;
	GLint       size;
	const void* offset; //  (void*)offsetof(Vertex, uv)
	bool        normalized{ false };
	bool        perInstance{ false };
};
void SpecifyVertexAttributes(size_t vertexSize, std::span<const VertexAttribute> attributes);

//=============================================================================
// Vertex Formats
//=============================================================================

struct VertexP final
{
	glm::vec3 position;
};
void SetVertexPAttributes();

struct VertexPNT final
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};
void SetVertexPNTAttributes();

//=============================================================================
// Buffer
//=============================================================================
GLuint CreateStaticVertexBuffer(GLsizeiptr size, const void* data);

//=============================================================================
// Textures
//=============================================================================