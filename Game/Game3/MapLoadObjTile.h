#pragma once

struct BlockModelInfo final
{
	std::string modelPath;
	glm::vec3 color{ 1.0f };

	glm::vec3 center{ 0.0f };
	glm::vec3 size{ 1.0f };
	glm::vec3 rotate{ 0.0f }; // Порядок вращения: Z (roll), Y (yaw), X (pitch) в радианах

	bool enablePlane[6] = { true, true, true, true, true, true };
};

void AddObjModel(const BlockModelInfo& modelInfo, 
	std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall,
	std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil,
	std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor);