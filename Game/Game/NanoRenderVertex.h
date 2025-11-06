#pragma once

struct QuadVertex final
{
	glm::vec2 position{ 0.0f };
	glm::vec2 texCoord{ 0.0f };

	static void SetVertexAttributes();
};

struct MeshVertex final
{
	glm::vec3 position{ 0.0f };
	glm::vec3 color{ 1.0f };
	glm::vec3 normal{ 0.0f };
	glm::vec2 texCoord{ 0.0f };
	glm::vec3 tangent{ 0.0f };
	glm::vec3 bitangent{ 0.0f };

	static void SetVertexAttributes();
};