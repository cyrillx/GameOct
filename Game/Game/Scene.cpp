#include "stdafx.h"
#include "Scene.h"
#include "NanoWindow.h"
#include "NanoIO.h"
#include "NanoLog.h"
//=============================================================================
bool Scene::Init()
{
	m_framebufferWidth = window::GetWidth();
	m_framebufferHeight = window::GetHeight();
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_entities.reserve(100000);

	if (!initMainShader())
		return false;
	
	//m_state.depthState.enable = true;
	//m_state.blendState.enable = true;
	//m_state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;

	m_shadowQuality = SHADOW_QUALITY::ULTRA;
	m_shadowMethod = SHADOW_METHOD::SECOND_DEPTH;
	m_bias = 0.0005f;
	m_show_depth_map = 0;
	m_orthoDimension = 10.0f;
	m_orthoProjection = glm::ortho(-m_orthoDimension, m_orthoDimension, -m_orthoDimension, m_orthoDimension, 0.1f, 100.0f);

	m_blinnPhong = CreateShaderProgram(io::ReadShaderCode("data/shaders/blinn_phong/vertex.glsl"), io::ReadShaderCode("data/shaders/blinn_phong/fragment.glsl"));
	m_pbr = CreateShaderProgram(io::ReadShaderCode("data/shaders/PBR/vertex.glsl"), io::ReadShaderCode("data/shaders/PBR/fragment.glsl"));
	m_postProcessing = CreateShaderProgram(io::ReadShaderCode("data/shaders/post_processing/vertex.glsl"), io::ReadShaderCode("data/shaders/post_processing/fragment.glsl"));
	
	if (!initShadowMappingShader())
		return false;

	m_multisample = { std::make_unique<Framebuffer>(true, true, true) };
	m_normal = { std::make_unique<Framebuffer>(true, false, true) };
	m_stdDepth = {
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false),
			std::make_unique<Framebuffer>(false)
	};

	m_multisample->AddAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
	m_multisample->AddAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);
	m_normal->AddAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);

	if (m_shadowQuality != SHADOW_QUALITY::OFF)
	{
		for (int i{ 0 }; i < 10; ++i)
		{
			m_stdDepth.at(i)->AddAttachment(AttachmentType::Texture, AttachmentTarget::Depth, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));
		}
	}
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
	updateSize();

	if (m_shadowQuality != SHADOW_QUALITY::OFF) 
	{
		// SHADOW PASS : directional & spot light sources
		directionalShadowPass();

		// COLOR PASS : multisampling
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
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
			if (m_mainShaderModelMatrixId >= 0)
				SetUniform(m_mainShaderModelMatrixId, m_entities[i]->modelMat);
			if (m_mainShaderNormalMatrixId >= 0)
				SetUniform(m_mainShaderNormalMatrixId, glm::transpose(glm::inverse(m_entities[i]->modelMat)));

			m_entities[i]->model.Draw(drawInfo);
		}
		// blit to normal framebuffer (resolve multisampling)

		// bind to default framebuffer

		// draw post processing quad
	}
	else // без теней
	{
		
	}
	
	if (m_gridAxis) m_gridAxis->Draw(m_perspective, m_camera->GetViewMatrix());

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
void Scene::SetShadowQuality(SHADOW_QUALITY quality)
{
	m_shadowQuality = quality;
	if (m_shadowQuality != SHADOW_QUALITY::OFF)
	{
		for (int i{ 0 }; i < 10; ++i)
			m_stdDepth.at(i)->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Depth, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));
	}
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
	glUseProgram(0);
	return true;
}
//=============================================================================
bool Scene::initShadowMappingShader()
{
	m_shadowMapping = CreateShaderProgram(io::ReadShaderCode("data/shaders/shadowMapping/vertex.glsl"), io::ReadShaderCode("data/shaders/shadowMapping/fragment.glsl"));
	if (!m_mainShader)
	{
		Fatal("Scene Shadow Mapping Shader failed!");
		return false;
	}

	glUseProgram(m_shadowMapping);
	SetUniform(GetUniformLocation(m_shadowMapping, "diffuseTexture"), 0);
	SetUniform(GetUniformLocation(m_shadowMapping, "hasDiffuse"), 1);
	m_shadowMappingShaderProjectionMatrixId = GetUniformLocation(m_shadowMapping, "projectionMatrix");
	m_shadowMappingShaderViewMatrixId = GetUniformLocation(m_shadowMapping, "viewMatrix");
	m_shadowMappingShaderModelMatrixId = GetUniformLocation(m_shadowMapping, "modelMatrix");
	glUseProgram(0);
	return true;
}
//=============================================================================
void Scene::updateSize()
{
	if (m_framebufferWidth != window::GetWidth() || m_framebufferHeight != window::GetHeight())
	{
		m_framebufferWidth = window::GetWidth();
		m_framebufferHeight = window::GetHeight();
		m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

		m_multisample->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
		m_multisample->UpdateAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);
		m_normal->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::Color, m_framebufferWidth, m_framebufferHeight);
	}
}
//=============================================================================
void Scene::directionalShadowPass()
{
	glViewport(0, 0, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));

	glUseProgram(m_shadowMapping);
	SetUniform(m_shadowMappingShaderProjectionMatrixId, m_orthoProjection);

	m_stdDepth[0]->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	auto& dlight = m_directionalLights[0];
	glm::vec3 lightPosition = dlight.GetPosition();
	glm::vec3 lightTarget = dlight.GetDirection();
	glm::mat4 lightView = glm::lookAt(lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	SetUniform(m_shadowMappingShaderViewMatrixId, lightView);

	// draw scene
	drawScene(drawScenePass::ShadowMapping);
}
//=============================================================================
void Scene::drawScene(drawScenePass scenePass)
{
	GLuint shader = 0;
	int modelMatrixId = -1;
	if (scenePass == drawScenePass::ShadowMapping)
	{
		shader = m_shadowMapping;
		modelMatrixId = m_shadowMappingShaderModelMatrixId;
	}
	else if (scenePass == drawScenePass::Temp)
	{
		shader = m_mainShader;
		modelMatrixId = m_mainShaderModelMatrixId;
	}

	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = true;
	drawInfo.mode = GL_TRIANGLES;
	for (size_t i = 0; i < m_maxEnts; i++)
	{
		if (modelMatrixId >= 0) SetUniform(modelMatrixId, m_entities[i]->modelMat);
		m_entities[i]->model.Draw(drawInfo);
	}
}
//=============================================================================