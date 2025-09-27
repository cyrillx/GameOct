#include "stdafx.h"
#include "Scene.h"
#include "NanoWindow.h"
//=============================================================================
void Scene::Init()
{
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
void Scene::Draw()
{
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	BindState(m_state);
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_gridAxis) m_gridAxis->Draw(m_perspective, GetCurrentCamera().GetViewMatrix());
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

