#include "stdafx.h"
#include "Scene.h"
#include "NanoWindow.h"
#include "NanoIO.h"
#include "NanoLog.h"
//=============================================================================
bool Scene::Init()
{
	m_entities.reserve(100000);

	if (!initMainShader())
		return false;
	
	m_state.depthState.enable = true;
	//m_state.blendState.enable = true;
	//m_state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;

	return true;
}
//=============================================================================
void Scene::Close()
{
	if (m_mainShader) glDeleteProgram(m_mainShader);
	m_mainShader = 0;
	m_gridAxis.reset();
}
//=============================================================================
void Scene::BindCamera(Camera* camera)
{
	m_camera = camera;
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
void Scene::Draw()
{
	assert(m_camera);
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	BindState(m_state);
	glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_mainShader);
	SetUniform(m_mainShaderProjectionMatrixId, m_perspective);
	SetUniform(m_mainShaderViewMatrixId, m_camera->GetViewMatrix());

	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = true;
	drawInfo.mode = GL_TRIANGLES;
	for (size_t i = 0; i < m_maxEnts; i++)
	{
		if ( m_mainShaderModelMatrixId >= 0 )
			SetUniform(m_mainShaderModelMatrixId, m_entities[i]->modelMat);
		if ( m_mainShaderNormalMatrixId >= 0 )
			SetUniform(m_mainShaderNormalMatrixId, glm::transpose(glm::inverse(m_entities[i]->modelMat)));

		m_entities[i]->model.Draw(drawInfo);
	}
	m_maxEnts = 0;

	if (m_gridAxis) m_gridAxis->Draw(m_perspective, m_camera->GetViewMatrix());
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
bool Scene::initMainShader()
{
	m_mainShader = CreateShaderProgram(io::ReadShaderCode("data/shaders/Model.vert"), io::ReadShaderCode("data/shaders/Model.frag"));
	if (!m_mainShader)
	{
		Fatal("Scene Main Shader failed!");
		return false;
	}

	glUseProgram(m_mainShader);
	SetUniform(GetUniformLocation(m_mainShader, "diffuseTexture"), 0);

	m_mainShaderProjectionMatrixId = GetUniformLocation(m_mainShader, "projectionMatrix");
	m_mainShaderViewMatrixId = GetUniformLocation(m_mainShader, "viewMatrix");
	m_mainShaderModelMatrixId = GetUniformLocation(m_mainShader, "modelMatrix");
	m_mainShaderNormalMatrixId = GetUniformLocation(m_mainShader, "normalMatrix");
	if (m_mainShaderProjectionMatrixId < 0 || m_mainShaderViewMatrixId < 0 || m_mainShaderModelMatrixId < 0 || m_mainShaderNormalMatrixId < 0)
	{
		Error("Scene MainShader failed!");
		//return false;
	}

	glUseProgram(0);
	return true;
}
//=============================================================================