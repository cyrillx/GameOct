#include "stdafx.h"
#include "Map.h"
#include "TileMap.h"
#include "MapLoadObjTile.h"
#define MAPCHUNKSIZE 30
size_t tempMap[MAPCHUNKSIZE][MAPCHUNKSIZE][MAPCHUNKSIZE] = { 0 };
//=============================================================================
void AddBox(
	const glm::vec3& center, float width, float height, float depth, const glm::vec3& color, std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall, std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil, std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor,
	bool enablePlane[6]);
//=============================================================================
void AddDiagonalBox(
	const glm::vec3& center, float width, float height, float depth, const glm::vec3& color, std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall, std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil, std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor,
	bool enablePlane[6]);
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
void optimizeMesh(std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices)
{
	// TODO:
}
//=============================================================================
bool MapChunk::Init()
{
	TileInfo tempTile;
	for (size_t y = 0; y < MAPCHUNKSIZE; y++)
	{
		for (size_t x = 0; x < MAPCHUNKSIZE; x++)
		{
			for (size_t z = 0; z < MAPCHUNKSIZE; z++)
			{
				tempMap[x][y][z] = NoTile;
			}
			tempTile.type = TileGeometryType::FullBox;
			tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
			tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
			tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");

			tempMap[x][y][0] = TileBank::AddTileInfo(tempTile);
		}
	}
	tempTile.type = TileGeometryType::None;

	tempMap[14][14][0] = NoTile;
	tempMap[14][15][0] = NoTile;
	tempMap[14][16][0] = NoTile;

	tempMap[15][14][0] = NoTile;
	tempMap[15][16][0] = NoTile;

	tempMap[16][14][0] = NoTile;
	tempMap[16][15][0] = NoTile;


	tempTile.type = TileGeometryType::NewBox;
	tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
	//tempTile.height = 1.5f;
	tempMap[15][15][0] = TileBank::AddTileInfo(tempTile);

	tempTile.type = TileGeometryType::NewBox2;
	tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
	//tempTile.height = 1.5f;
	tempMap[16][17][1] = TileBank::AddTileInfo(tempTile);

	tempTile.type = TileGeometryType::NewBox;
	tempTile.textureWall = textures::LoadTexture2D("data/tiles/grass01_wall.png", ColorSpace::Linear, true);
	tempTile.textureCeil = textures::LoadTexture2D("data/tiles/grass01_ceil.png");
	tempTile.textureFloor = textures::LoadTexture2D("data/tiles/grass01.png");
	//tempTile.height = 1.5f;
	tempMap[18][17][1] = TileBank::AddTileInfo(tempTile);


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
	std::vector<MeshInfo> meshInfo;
	BlockModelInfo blockModelInfo{};


	const float mapOffset = MAPCHUNKSIZE / 2;
	for (size_t iy = 0; iy < MAPCHUNKSIZE; iy++)
	{
		for (size_t ix = 0; ix < MAPCHUNKSIZE; ix++)
		{
			for (size_t iz = 0; iz < MAPCHUNKSIZE; iz++)
			{
				if (tempMap[ix][iy][iz] == NoTile) continue;

				auto id = *TileBank::GetTileInfo(tempMap[ix][iy][iz]);
				if (id.type == TileGeometryType::None) continue;

				float x = float(ix) - mapOffset;
				float y = float(iy) - mapOffset;
				float z = float(iz);

				bool enablePlane[6] = { true }; // TODO: убрать невидимое

				size_t idWall = addMeshInfo(meshInfo, id.textureWall);
				size_t idFloor = addMeshInfo(meshInfo, id.textureFloor);
				size_t idCeil = addMeshInfo(meshInfo, id.textureCeil);

				float heightBlock = id.height;

				glm::vec3 center = glm::vec3(x, z + heightBlock / 2.0f, y);

				if (id.type == TileGeometryType::FullBox)
				{
					blockModelInfo.modelPath = "data/tiles/test/123.obj";
					blockModelInfo.color = id.color;
					blockModelInfo.center = center;
					blockModelInfo.size = { 1.0f, heightBlock, 1.0f };
					blockModelInfo.rotate = glm::vec3(0.0f);
					for (size_t i = 0; i < 6; i++)
					{
						blockModelInfo.enablePlane[i] = enablePlane[i];
					}

					AddObjModel(blockModelInfo,
						meshInfo[idWall].vertices, meshInfo[idWall].indices,
						meshInfo[idCeil].vertices, meshInfo[idCeil].indices,
						meshInfo[idFloor].vertices, meshInfo[idFloor].indices);
				}
				else if (id.type == TileGeometryType::NewBox)
				{
					blockModelInfo.modelPath = "data/tiles/test/222.obj";
					blockModelInfo.color = id.color;
					blockModelInfo.center = center;
					blockModelInfo.size = { 1.0f, heightBlock, 1.0f };
					blockModelInfo.rotate = glm::vec3(0.0f);
					for (size_t i = 0; i < 6; i++)
					{
						blockModelInfo.enablePlane[i] = enablePlane[i];
					}

					AddObjModel(blockModelInfo,
						meshInfo[idWall].vertices, meshInfo[idWall].indices,
						meshInfo[idCeil].vertices, meshInfo[idCeil].indices,
						meshInfo[idFloor].vertices, meshInfo[idFloor].indices);
				}
				else if (id.type == TileGeometryType::NewBox2)
				{
					blockModelInfo.modelPath = "data/tiles/test/333.obj";
					blockModelInfo.color = id.color;
					blockModelInfo.center = center;
					blockModelInfo.size = { 1.0f, heightBlock, 1.0f };
					blockModelInfo.rotate = glm::vec3(0.0f);
					blockModelInfo.rotate.y = glm::radians(45.0f);
					for (size_t i = 0; i < 6; i++)
					{
						blockModelInfo.enablePlane[i] = enablePlane[i];
					}

					AddObjModel(blockModelInfo, 
						meshInfo[idWall].vertices, meshInfo[idWall].indices,
						meshInfo[idCeil].vertices, meshInfo[idCeil].indices,
						meshInfo[idFloor].vertices, meshInfo[idFloor].indices);
				}
			}
		}
	}

	for (size_t i = 0; i < meshInfo.size(); i++)
	{
		optimizeMesh(meshInfo[i].vertices, meshInfo[i].indices);

		m_vertCount += meshInfo[i].vertices.size();
		m_indexCount += meshInfo[i].indices.size();
	}

	m_model.model.Create(meshInfo);
}
//=============================================================================