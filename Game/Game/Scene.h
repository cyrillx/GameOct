#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "Light.h"
#include "GridAxis.h"

struct Entity final
{
	const AABB& GetAABB() const noexcept { return model.GetAABB(); }

	Model     model;
	glm::mat4 modelMat{ glm::mat4(1.0f) };
	int       modelMatrixId{ -1 };
	int       normalMatrixId{ -1 };

	bool      visible{ true };


};

class Scene final
{
public:
	void Init();
	void Close();
	void Draw(GLuint shaderId);

#pragma region [ Entity ]

	void BindEntity(Entity* ent);

#pragma endregion


#pragma region [ Camera ]

	Camera& GetCurrentCamera() { return m_cameras[m_currentCameraId]; }
	const Camera& GetCurrentCamera() const { return m_cameras[m_currentCameraId]; }
	void SetCurrentCamera(size_t id) { assert(id < m_cameras.size()); m_currentCameraId = id; }
	void AddCamera(const Camera& camera) { m_cameras.emplace_back(camera); }

	const glm::mat4& GetPerspective() const { return m_perspective; }

#pragma endregion

	void SetGridAxis(int gridDim);

private:
	GLState                                m_state;

	glm::mat4                              m_perspective{ 1.0f };
	size_t                                 m_currentCameraId{ 0 };
	std::vector<Camera>                    m_cameras{ 1u };

	std::vector<Entity*>                   m_entities;
	size_t                                 m_maxEnts{ 0 };

	std::vector<DirectionalLight>          m_directionalLights;
	std::vector<SpotLight>                 m_spotLights;

	std::unique_ptr<GridAxis>              m_gridAxis;
};