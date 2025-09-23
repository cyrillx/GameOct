#include "stdafx.h"
#include "NanoOpenGL3.h"
//=============================================================================
int GetUniformLocation(GLuint program, std::string_view name)
{
	return glGetUniformLocation(program, name.data());
}
//=============================================================================
void SetUniform(GLuint id, float s)
{
	glUniform1f(id, s);
}
//=============================================================================
void SetUniform(GLuint id, int s)
{
	glUniform1i(id, s);
}
//=============================================================================
void SetUniform(GLuint id, const glm::vec2& v)
{
	glUniform2fv(id, 1, glm::value_ptr(v));
}
//=============================================================================
void SetUniform(GLuint id, std::span<const glm::vec2> v)
{
	GLsizei count = v.size();
	glUniform2fv(id, count, glm::value_ptr(v[0]));
}
//=============================================================================
void SetUniform(GLuint id, const glm::vec3& v)
{
	glUniform3fv(id, 1, glm::value_ptr(v));
}
//=============================================================================
void SetUniform(GLuint id, std::span<const glm::vec3> v)
{
	GLsizei count = v.size();
	glUniform3fv(id, count, glm::value_ptr(v[0]));
}
//=============================================================================
void SetUniform(GLuint id, const glm::vec4& v)
{
	glUniform4fv(id, 1, glm::value_ptr(v));
}
//=============================================================================
void SetUniform(GLuint id, std::span<const glm::vec4> v)
{
	GLsizei count = v.size();
	glUniform4fv(id, count, glm::value_ptr(v[0]));
}
//=============================================================================
void SetUniform(GLuint id, const glm::quat& v)
{
	glUniform4fv(id, 1, glm::value_ptr(v));
}
//=============================================================================
void SetUniform(GLuint id, const glm::mat3& m)
{
	glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(m));
}
//=============================================================================
void SetUniform(GLuint id, const glm::mat4& m)
{
	glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(m));
}
//=============================================================================
void SpecifyVertexAttributes(size_t vertexSize, std::span<const VertexAttribute> attributes)
{
	assert(vertexSize > 0);
	assert(attributes.size() > 0);

	for (size_t i = 0; i < attributes.size(); i++)
	{
		const auto& attr = attributes[i];
		const GLuint index = static_cast<GLuint>(i);

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, attr.size, attr.type, attr.normalized ? GL_TRUE : GL_FALSE, vertexSize, attr.offset);
		glVertexAttribDivisor(index, attr.perInstance ? 1 : 0);
	}
}
//=============================================================================
void SetVertexPAttributes()
{
	const size_t vertexSize = sizeof(VertexP);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(VertexP, position)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
void SetVertexPNTAttributes()
{
	const size_t vertexSize = sizeof(VertexPNT);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(VertexPNT, position)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(VertexPNT, normal)},
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(VertexPNT, texcoord)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
GLuint CreateStaticVertexBuffer(GLsizeiptr size, const void* data)
{
	GLuint vbo{ 0 };
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}
//=============================================================================