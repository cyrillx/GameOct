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
	bool      visible{ true };
};

class Scene final
{
public:
	bool Init();
	void Close();
	void Draw();


	void BindCamera(Camera* camera);
	void BindEntity(Entity* ent);

	void SetGridAxis(int gridDim);

private:
	bool initMainShader();

	GLState                                m_state;
	GLuint                                 m_mainShader{ 0 };
	int                                    m_mainShaderProjectionMatrixId{ -1 };
	int                                    m_mainShaderViewMatrixId{ -1 };
	int                                    m_mainShaderModelMatrixId{ -1 };
	int                                    m_mainShaderNormalMatrixId{ -1 };

	glm::mat4                              m_perspective{ 1.0f };
	Camera*                                m_camera{ nullptr };

	std::vector<Entity*>                   m_entities;
	size_t                                 m_maxEnts{ 0 };

	std::vector<DirectionalLight>          m_directionalLights;
	std::vector<SpotLight>                 m_spotLights;

	std::unique_ptr<GridAxis>              m_gridAxis;
};