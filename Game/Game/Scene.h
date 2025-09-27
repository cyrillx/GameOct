#pragma once

#include "NanoRender.h"
#include "NanoScene.h"
#include "Light.h"
#include "GridAxis.h"

class ModelObject final
{

};

class Scene final
{
public:
	void Init();
	void Close();
	void Draw();

	//-------------------------------------------------------------------------
	// Camera
	//-------------------------------------------------------------------------
#pragma region [ Camera ]

	Camera& GetCurrentCamera() { return m_cameras[m_currentCameraId]; }
	const Camera& GetCurrentCamera() const { return m_cameras[m_currentCameraId]; }
	void SetCurrentCamera(size_t id) { assert(id < m_cameras.size()); m_currentCameraId = id; }
	void AddCamera(const Camera& camera) { m_cameras.emplace_back(camera); }

	const glm::mat4& GetPerspective() const { return m_perspective; }

#pragma endregion

	void SetGridAxis(int gridDim);

private:
	GLState                       m_state;

	glm::mat4                     m_perspective{ 1.0f };

	size_t                        m_currentCameraId{ 0 };
	std::vector<Camera>           m_cameras{ 1u };

	std::vector<ModelObject>      m_objModel;

	std::vector<DirectionalLight> m_directionalLights;
	std::vector<SpotLight>        m_spotLights;

	std::unique_ptr<GridAxis>     m_gridAxis;
};