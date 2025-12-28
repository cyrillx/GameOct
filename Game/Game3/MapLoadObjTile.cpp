#include "stdafx.h"
//=============================================================================
// Псевдокод (подробно):
// 1. Загрузить OBJ через tinyobj::LoadObj.
// 2. Для каждой формы (shapes[s]) определить, к какому массиву вершин/индексов относится (bottom/top/others).
// 3. Использовать структуру сравнения для tinyobj::index_t, чтобы можно было использовать std::map как кэш уникальных вершин.
//    - Сравнивать по vertex_index, затем texcoord_index, затем normal_index.
// 4. Для каждого индекса в shapes[s].mesh.indices:
//    - Если индекс не найден в кэше, создать MeshVertex, заполнить позицию/нормаль/uv (с проверками границ), цвет/тангенты по умолчанию,
//      добавить в вектор вершин и сохранить соответствие index->new_index в кэше.
//    - Добавить в выходной массив индексов значение из кэша.
// 5. Обрабатывать предупреждения и ошибки tinyobj.
//=============================================================================

struct IndexLess
{
	bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const
	{
		if (a.vertex_index != b.vertex_index) return a.vertex_index < b.vertex_index;
		if (a.texcoord_index != b.texcoord_index) return a.texcoord_index < b.texcoord_index;
		return a.normal_index < b.normal_index;
	}
};

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

		// Используем std::map с пользовательской функцией сравнения для tinyobj::index_t
		std::map<tinyobj::index_t, unsigned int, IndexLess> vertex_cache;

		for (size_t f = 0; f < shapes[s].mesh.indices.size(); f += 3)
		{
			// Получаем 3 индекса для треугольника
			tinyobj::index_t idx0 = shapes[s].mesh.indices[f + 0];
			tinyobj::index_t idx1 = shapes[s].mesh.indices[f + 1];
			tinyobj::index_t idx2 = shapes[s].mesh.indices[f + 2];

			// Инвертируем порядок, чтобы изменить winding (CW <-> CCW)
			// Было: idx0, idx1, idx2
			// Стало: idx0, idx2, idx1
			const std::array<tinyobj::index_t, 3> reversed_indices = { idx0, idx2, idx1 };

			for (const auto& idx : reversed_indices)
			{
				// Проверяем, была ли уже такая вершина обработана
				auto it = vertex_cache.find(idx);
				if (it == vertex_cache.end())
				{
					// Новая вершина
					MeshVertex vertex{};

					// Позиция
					if (idx.vertex_index >= 0 && static_cast<size_t>(idx.vertex_index) * 3 + 2 < attrib.vertices.size())
					{
						vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
						vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
						vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];
					}

					// Нормаль
					if (idx.normal_index >= 0 && static_cast<size_t>(idx.normal_index) * 3 + 2 < attrib.normals.size())
					{
						vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
						vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
						vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];
					}

					// UV-координаты
					if (idx.texcoord_index >= 0 && static_cast<size_t>(idx.texcoord_index) * 2 + 1 < attrib.texcoords.size())
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

					// Добавляем индекс в массив индексов
					indices->push_back(new_index);
				}
				else
				{
					// Добавляем индекс из кэша
					indices->push_back(it->second);
				}
			}
		}
	}
}
//=============================================================================