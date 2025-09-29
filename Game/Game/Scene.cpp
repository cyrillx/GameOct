#include "stdafx.h"
#include "Scene.h"
#include "NanoWindow.h"
//=============================================================================
void Scene::Init()
{
	m_entities.reserve(100000);

	m_state.depthState.enable = true;
	//m_state.blendState.enable = true;
	//m_state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;
}
//=============================================================================
void Scene::Close()
{
	m_gridAxis.reset();
}
//=============================================================================
void Scene::BindEntity(Entity* ent)
{
	if (m_maxEnts >= m_entities.size())
		m_entities.push_back(ent);
	else
		m_entities[m_maxEnts] = ent;

	m_maxEnts++;
}
//=============================================================================
void Scene::Draw(GLuint shaderId)
{
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	BindState(m_state);
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderId);
	SetUniform(GetUniformLocation(shaderId, "projectionMatrix"), GetPerspective());
	SetUniform(GetUniformLocation(shaderId, "viewMatrix"), GetCurrentCamera().GetViewMatrix());

	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = true;
	drawInfo.mode = GL_TRIANGLES;

	for (size_t i = 0; i < m_maxEnts; i++)
	{
		if (m_entities[i]->modelMatrixId > -1)
			SetUniform(m_entities[i]->modelMatrixId, m_entities[i]->modelMat);

		if (m_entities[i]->normalMatrixId > -1)
			SetUniform(m_entities[i]->normalMatrixId, glm::transpose(glm::inverse(m_entities[i]->modelMat)));

		m_entities[i]->model.Draw(drawInfo);
	}

	if (m_gridAxis) m_gridAxis->Draw(m_perspective, GetCurrentCamera().GetViewMatrix());

	m_maxEnts = 0;
}
//=============================================================================
void Scene::SetGridAxis(int gridDim)
{
	if (gridDim == 0)
		m_gridAxis.reset();
	else
		m_gridAxis = std::make_unique<GridAxis>(gridDim);
}
//=============================================================================

