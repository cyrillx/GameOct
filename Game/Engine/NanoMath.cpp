#include "stdafx.h"
#include "NanoMath.h"
//=============================================================================
glm::mat4 GetTransformMatrix(const glm::vec3& position, const glm::vec3& rotation, float scale)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(scale));
	return transform;
}
//=============================================================================
void AABB::Set(const std::vector<glm::vec3>& vertexData, const std::vector<uint32_t>& indexData)
{
	if (indexData.size() > 0)
	{
		for (size_t index_id = 0; index_id < indexData.size(); index_id++)
		{
			CombinePoint(vertexData[indexData[index_id]]);
		}
	}
	else
	{
		for (size_t vertex_id = 0; vertex_id < vertexData.size(); vertex_id++)
		{
			CombinePoint(vertexData[vertex_id]);
		}
	}
}
//=============================================================================