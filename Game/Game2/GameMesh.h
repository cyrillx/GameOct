#pragma once

struct GameMeshBufferData final
{
	unsigned int triangles;
	unsigned int numFaces{ 0 };
	unsigned int numVertices{ 0 };
	unsigned int* faceArray{ nullptr };

	float* vertexArray{ nullptr };
	float* normalArray{ nullptr };
	float* colorArray{ nullptr };
	float* textCoordArray{ nullptr };
	float* tangentsArray{ nullptr };
};

struct GameBuffer final
{
	VertexArrayHandle vao{ 0 };
	BufferHandle      vbo{ 0 };
	BufferHandle      ebo{ 0 };
};

class GameMesh
{
public:
	GameMesh() = default;
	GameMesh(const std::string& r) : m_fileRoute(r) { ImportFile(); }

	virtual ~GameMesh()
	{
		delete[] m_vaos;
		delete[] m_bufferData;
	}

	std::string GetFileRoute() const { return m_fileRoute; }
	
	void SetCastShadows(bool c) { m_castShadows = c; }
	bool GetCastShadows() const { return m_castShadows; }

	bool IsInstanced() const { return m_isInstancedMesh; }

	unsigned int GetNumberOfMeshes() const { return m_meshNumber; }

	GameMeshBufferData GetMeshBuffertData(unsigned int idx = 0) { return m_bufferData[idx]; }

	void SetDirty() { m_isDirty = true; }

	virtual void GenerateBuffers();

	virtual void Draw();
	virtual void Draw(int meshIdx);

	void ImportFile();

protected:
	std::string         m_fileRoute{ "" };
	unsigned int        m_meshNumber{ 0 };
	GameBuffer*         m_vaos{ nullptr };
	GameMeshBufferData* m_bufferData{ nullptr };

	bool                m_isInstancedMesh{ false };
	bool                m_castShadows{ true };

	bool                m_isDirty{ true };
};