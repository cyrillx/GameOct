#pragma once

struct BlockModelInfo final
{
	BlockModelInfo()
	{
		for (size_t i = 0; i < 6; i++)
		{
			enablePlane[i] = true;
		}
	}

	std::string modelPath;
	glm::vec3 color{ 1.0f };

	glm::vec3 center{ 0.0f };
	glm::vec3 size{ 1.0f };
	glm::vec3 rotate{ 0.0f }; // Порядок вращения: Z (roll), Y (yaw), X (pitch) в радианах

	// forward, right, back, left, top, bottom
	bool enablePlane[6];
};

void AddObjModel(const BlockModelInfo& modelInfo, 
	std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall,
	std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil,
	std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor);