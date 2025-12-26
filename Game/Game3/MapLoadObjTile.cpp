#include "stdafx.h"
//=============================================================================
void AddObjModel(const glm::vec3& center, float width, float height, float depth, const glm::vec3& color, std::vector<MeshVertex>& verticesWall, std::vector<unsigned int>& indicesWall, std::vector<MeshVertex>& verticesCeil, std::vector<unsigned int>& indicesCeil, std::vector<MeshVertex>& verticesFloor, std::vector<unsigned int>& indicesFloor, bool enablePlane[6])
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "data/tiles/test/123.obj");
	if (!ret)
	{
		Fatal("Error loading OBJ file: " + err);
		return;
	}
	if (!warn.empty())
	{
		Warning("TinyObjLoader Warning: " + warn);
	}

	// Проходим по всем формам (мешам)
	for (size_t s = 0; s < shapes.size(); s++)
	{
		//mesh.material_id = shapes[s].mesh.material_ids[0]; // Берем ID материала для текущего меша

		const auto& name = shapes[s].name;
		std::vector<MeshVertex>* vertices = nullptr;
		std::vector<unsigned int>* indices = nullptr;

		if (name == "bottom")
		{
			vertices = &verticesCeil;
			indices = &indicesCeil;	
		}
		else if (name == "top")
		{
			vertices = &verticesFloor;
			indices = &indicesFloor;
		}
		else
		{
			vertices = &verticesWall;
			indices = &indicesWall;
		}

		// Используем map для отслеживания уже обработанных вершин и избежания дубликатов
		std::map<tinyobj::index_t, unsigned int> vertex_cache;

		for (size_t f = 0; f < shapes[s].mesh.indices.size(); f++)
		{
			tinyobj::index_t idx = shapes[s].mesh.indices[f];

			// Проверяем, была ли уже такая вершина обработана
			if (vertex_cache.find(idx) == vertex_cache.end())
			{
				// Новая вершина
				MeshVertex vertex{};

				// Позиция
				if (idx.vertex_index >= 0 && idx.vertex_index * 3 + 2 < attrib.vertices.size())
				{
					vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
					vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
					vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];
				}

				// Нормаль
				if (idx.normal_index >= 0 && idx.normal_index * 3 + 2 < attrib.normals.size())
				{
					vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
					vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
					vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];
				}

				// UV-координаты
				if (idx.texcoord_index >= 0 && idx.texcoord_index * 2 + 1 < attrib.texcoords.size())
				{
					vertex.texCoord.x = attrib.texcoords[2 * idx.texcoord_index + 0];
					vertex.texCoord.y = attrib.texcoords[2 * idx.texcoord_index + 1];
				}

				// Цвет (установим по умолчанию, так как OBJ обычно не хранит цвет вершины)
				vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);

				// Тангент и битангент (установим по умолчанию, так как их нужно вычислять отдельно)
				vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
				vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

				// Добавляем вершину в массив
				unsigned int new_index = static_cast<unsigned int>(vertices->size());
				vertices->push_back(vertex);

				// Сохраняем индекс новой вершины в кэше
				vertex_cache[idx] = new_index;
			}

			// Добавляем индекс из кэша
			indices->push_back(vertex_cache[idx]);
		}
	}
}
//=============================================================================