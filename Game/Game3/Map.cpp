#include "stdafx.h"
#include "Map.h"
#include "TileMap.h"
#define MAPCHUNKSIZE 10
TileInfo tempMap[MAPCHUNKSIZE][MAPCHUNKSIZE][MAPCHUNKSIZE];
//=============================================================================
void AddBox(
	const glm::vec3& center, float width, float height, float depth, const glm::vec3& color, std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall, std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil, std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor);
//=============================================================================
bool MapChunk::Init()
{
	for (size_t y = 0; y < MAPCHUNKSIZE; y++)
	{
		for (size_t x = 0; x < MAPCHUNKSIZE; x++)
		{
			for (size_t z = 0; z < MAPCHUNKSIZE; z++)
			{
				tempMap[x][y][z].type = TileGeometryType::None;
			}
			tempMap[x][y][0].type = TileGeometryType::FullBox1;
			tempMap[x][y][0].textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
			tempMap[x][y][0].textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
			tempMap[x][y][0].textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
		}
	}

	generateBufferMap();

	return true;
}
//=============================================================================
void MapChunk::Close()
{
}
//=============================================================================
size_t addMeshInfo(std::vector<MeshInfo>& meshInfo, Texture2D texId)
{
	for (size_t i = 0; i < meshInfo.size(); i++)
	{
		if (meshInfo[i].material->diffuseTextures[0] == texId)
		{
			return i;
		}
	}

	MeshInfo nmi{};
	nmi.material = Material();
	nmi.material->diffuseTextures.push_back(texId);
	meshInfo.push_back(nmi);
	return meshInfo.size() - 1;
}
//=============================================================================
void MapChunk::generateBufferMap()
{
	std::vector<MeshInfo> meshInfo;

	const float mapOffset = MAPCHUNKSIZE / 2;
	for (size_t iy = 0; iy < MAPCHUNKSIZE; iy++)
	{
		for (size_t ix = 0; ix < MAPCHUNKSIZE; ix++)
		{
			for (size_t iz = 0; iz < MAPCHUNKSIZE; iz++)
			{
				auto id = tempMap[ix][iy][iz];
				if (id.type == TileGeometryType::None) continue;

				size_t idWall = addMeshInfo(meshInfo, id.textureWall);
				size_t idFloor = addMeshInfo(meshInfo, id.textureFloor);
				size_t idCeil = addMeshInfo(meshInfo, id.textureCeil);

				float x = float(ix) - mapOffset;
				float y = float(iy) - mapOffset;
				float z = float(iz) + 0.5f;

				AddBox(glm::vec3(x, z, y), 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), 
					meshInfo[idWall].vertices, meshInfo[idWall].indices,
					meshInfo[idCeil].vertices, meshInfo[idCeil].indices,
					meshInfo[idFloor].vertices, meshInfo[idFloor].indices);
			}
		}
	}
	m_model.model.Create(meshInfo);
}
//=============================================================================