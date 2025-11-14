#include "stdafx.h"
#include "NanoOpenGL3Advance.h"
#include "NanoLog.h"
//=============================================================================
void SetUniform(int id, bool b)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), b);
}
//=============================================================================
void SetUniform(int id, float s)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), s);
}
//=============================================================================
void SetUniform(int id, int s)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), s);
}
//=============================================================================
void SetUniform(int id, unsigned s)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), s);
}
//=============================================================================
void SetUniform(int id, const glm::vec2& v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), v);
}
//=============================================================================
void SetUniform(int id, std::span<const glm::vec2> v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), v);
}
//=============================================================================
void SetUniform(int id, const glm::vec3& v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), v);
}
//=============================================================================
void SetUniform(int id, std::span<const glm::vec3> v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), v);
}
//=============================================================================
void SetUniform(int id, const glm::vec4& v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), v);
}
//=============================================================================
void SetUniform(int id, std::span<const glm::vec4> v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), v);
}
//=============================================================================
void SetUniform(int id, const glm::quat& v)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), v);
}
//=============================================================================
void SetUniform(int id, const glm::mat3& m)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), m);
}
//=============================================================================
void SetUniform(int id, const glm::mat4& m)
{
	if (id < 0)
	{
		Error("Uniform error");
		return;
	}
	SetUniform(static_cast<GLuint>(id), m);
}
//=============================================================================