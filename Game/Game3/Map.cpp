#include "stdafx.h"
#include "Map.h"
#define MAPCHUNKSIZE 20
int tempMap[MAPCHUNKSIZE][MAPCHUNKSIZE];
//=============================================================================
void addPlane(std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices, float width = 1.0f, float depth = 1.0f, uint32_t subdivisions_width = 1, uint32_t subdivisions_depth = 1, glm::vec3 offset = { 0.0f, 0.0f, 0.0f })
{
	const uint32_t startVertexIndex = static_cast<uint32_t>(vertices.size());

	// Генерация вершин
	for (uint32_t z = 0; z <= subdivisions_depth; ++z) {
		for (uint32_t x = 0; x <= subdivisions_width; ++x) {
			float u = static_cast<float>(x) / subdivisions_width;
			float v = static_cast<float>(z) / subdivisions_depth;

			MeshVertex vertex;
			vertex.position.x = (u - 0.5f) * width + offset.x;
			vertex.position.y = offset.y;
			vertex.position.z = (v - 0.5f) * depth + offset.z;

			vertex.normal = { 0.0f, 1.0f, 0.0f };
			vertex.color = { 1.0f, 1.0f, 1.0f }; // Можно изменить
			vertex.texCoord = { u, v };

			// Пример вычисления касательных (упрощённо)
			vertex.tangent = { 1.0f, 0.0f, 0.0f };
			vertex.bitangent = { 0.0f, 0.0f, 1.0f };

			vertices.push_back(vertex);
		}
	}

	// Генерация индексов (треугольники)
	for (uint32_t z = 0; z < subdivisions_depth; ++z) {
		for (uint32_t x = 0; x < subdivisions_width; ++x) {
			uint32_t a = startVertexIndex + (z * (subdivisions_width + 1)) + x;
			uint32_t b = a + 1;
			uint32_t c = a + (subdivisions_width + 1);
			uint32_t d = c + 1;

			indices.push_back(a);
			indices.push_back(c);
			indices.push_back(b);

			indices.push_back(b);
			indices.push_back(c);
			indices.push_back(d);
		}
	}
}
//=============================================================================
bool MapChunk::Init()
{
	for (size_t y = 0; y < MAPCHUNKSIZE; y++)
	{
		for (size_t x = 0; x < MAPCHUNKSIZE; x++)
		{
			tempMap[x][y] = 0;
		}
	}

	MeshInfo meshInfo[10];
	meshInfo[0].material = Material();
	meshInfo[0].material->diffuseTextures.push_back(textures::LoadTexture2D("data/textures/grass_diffuse.png"));

	for (size_t y = 0; y < MAPCHUNKSIZE; y++)
	{
		for (size_t x = 0; x < MAPCHUNKSIZE; x++)
		{
			if (tempMap[x][y] == 0)
			{
				addPlane(meshInfo[0].vertices, meshInfo[0].indices, 1.0f, 1.0f, 1, 1, {x*2, 0.0f, y*2});
			}
		}
	}
	m_model.model.Create(meshInfo[0]);

	return true;
}
//=============================================================================
void MapChunk::Close()
{
}
//=============================================================================