#pragma once

#include "GameModel.h"

struct BlockModelInfo;
struct TileInfo;

class MapChunk final
{
public:
	bool Init();
	void Close();

	GameModel* GetModel() noexcept { return &m_model; }
	size_t GetVertexCount() const { return m_vertCount; }
	size_t GetIndexCount() const { return m_indexCount; }
private:
	void generateBufferMap();
	void setVisibleBlock(const TileInfo& ti, BlockModelInfo& blockModelInfo, size_t x, size_t y, size_t z);

	GameModel m_model;

	size_t m_vertCount;
	size_t m_indexCount;
};