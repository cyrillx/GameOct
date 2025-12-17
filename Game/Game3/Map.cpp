#include "stdafx.h"
#include "Map.h"
#define MAPCHUNKSIZE 10
int tempMap[MAPCHUNKSIZE][MAPCHUNKSIZE];
//=============================================================================
void AddBox(const glm::vec3& center, float width, float height, float depth, const glm::vec3& color, std::vector<MeshVertex>& vertices, std::vector<unsigned int>& indices);
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

	tempMap[5][6] = 1;
	tempMap[4][5] = 1;
	tempMap[5][5] = 1;
	tempMap[5][4] = 1;
	tempMap[6][5] = 1;

	generateBufferMap();

	return true;
}
//=============================================================================
void MapChunk::Close()
{
}
//=============================================================================
void MapChunk::generateBufferMap()
{
	std::vector<MeshInfo> meshInfo(10);
	meshInfo[0].material = Material();
	meshInfo[0].material->diffuseTextures.push_back(textures::LoadTexture2D("data/pics/wall1.png"));
	meshInfo[1].material = Material();
	meshInfo[1].material->diffuseTextures.push_back(textures::LoadTexture2D("data/pics/tile2.png"));

	const float mapOffset = MAPCHUNKSIZE / 2;
	for (size_t iy = 0; iy < MAPCHUNKSIZE; iy++)
	{
		for (size_t ix = 0; ix < MAPCHUNKSIZE; ix++)
		{
			size_t id = tempMap[ix][iy];
			if (id == 99) continue;

			float x = float(ix) - mapOffset;
			float y = float(iy) - mapOffset;

			AddBox(glm::vec3(x, 0.5f, y), 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), meshInfo[id].vertices, meshInfo[id].indices);
		}
	}
	m_model.model.Create(meshInfo);
}
//=============================================================================