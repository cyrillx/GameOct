#include "stdafx.h"
#include "NanoRenderVertex.h"
#include "NanoOpenGL3.h"
//=============================================================================
void QuadVertex::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(QuadVertex);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(QuadVertex, position)},
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(QuadVertex, texCoord)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================
void MeshVertex::SetVertexAttributes()
{
	const size_t vertexSize = sizeof(MeshVertex);
	const VertexAttribute attributes[] =
	{
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, position)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, color)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, normal)},
		{.type = GL_FLOAT, .size = 2, .offset = (void*)offsetof(MeshVertex, texCoord)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, tangent)},
		{.type = GL_FLOAT, .size = 3, .offset = (void*)offsetof(MeshVertex, bitangent)},
	};
	SpecifyVertexAttributes(vertexSize, attributes);
}
//=============================================================================