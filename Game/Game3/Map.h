#pragma once

#include "GameModel.h"

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
	GameModel m_model;

	size_t m_vertCount;
	size_t m_indexCount;
};