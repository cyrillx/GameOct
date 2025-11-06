#include "stdafx.h"
#include "RPGeometry.h"
#include "NanoWindow.h"
#include "NanoLog.h"
#include "GameScene.h"
//=============================================================================
bool RPGeometry::Init(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_program = LoadShaderProgram("data/shaders/geometry/vertex.glsl", "data/shaders/geometry/fragment.glsl");
	if (!m_program)
	{
		Fatal("Scene Geometry RenderPass Shader failed!");
		return false;
	}

	glUseProgram(m_program);

	m_projectionMatrixId = GetUniformLocation(m_program, "projectionMatrix");
	m_viewMatrixId = GetUniformLocation(m_program, "viewMatrix");
	m_modelMatrixId = GetUniformLocation(m_program, "modelMatrix");

	glUseProgram(0); // TODO: возможно вернуть прошлую версию шейдера

	m_fbo = { std::make_unique<Framebuffer>(true, false, true) };

	m_fbo->AddAttachment(AttachmentType::Texture, AttachmentTarget::ColorRGB, m_framebufferWidth, m_framebufferHeight);
	m_fbo->AddAttachment(AttachmentType::Texture, AttachmentTarget::ColorRGB, m_framebufferWidth, m_framebufferHeight);
	m_fbo->AddAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);

	SamplerStateInfo samperCI{};
	samperCI.minFilter = TextureFilter::Nearest;
	samperCI.magFilter = TextureFilter::Nearest;
	m_sampler = CreateSamplerState(samperCI);

	return true;
}
//=============================================================================
void RPGeometry::Close()
{
	m_fbo.reset();
	glDeleteProgram(m_program);
	glDeleteSamplers(1, &m_sampler);
}
//=============================================================================
void RPGeometry::Resize(uint16_t framebufferWidth, uint16_t framebufferHeight)
{
	if (m_framebufferWidth == framebufferWidth && m_framebufferHeight == framebufferHeight)
		return;

	m_framebufferWidth = framebufferWidth;
	m_framebufferHeight = framebufferHeight;
	m_perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

	m_fbo->UpdateAttachment(AttachmentType::Texture, AttachmentTarget::ColorRGBA, m_framebufferWidth, m_framebufferHeight);
	m_fbo->UpdateAttachment(AttachmentType::RenderBuffer, AttachmentTarget::DepthStencil, m_framebufferWidth, m_framebufferHeight);
}
//=============================================================================
void RPGeometry::Draw(const std::vector<GameObject*>& gameObject, size_t numGameObject, Camera* camera)
{
	m_fbo->Bind();
	glViewport(0, 0, static_cast<int>(m_framebufferWidth), static_cast<int>(m_framebufferHeight));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT/* | GL_STENCIL_BUFFER_BIT*/);

	glUseProgram(m_program);
	SetUniform(m_projectionMatrixId, m_perspective);
	SetUniform(m_viewMatrixId, camera->GetViewMatrix());

	glBindSampler(0, m_sampler);
	drawScene(gameObject, numGameObject);
	glBindSampler(0, 0);
}
//=============================================================================
void RPGeometry::drawScene(const std::vector<GameObject*>& gameObject, size_t numGameObject)
{
	ModelDrawInfo drawInfo;
	drawInfo.bindMaterials = true;
	drawInfo.mode = GL_TRIANGLES;
	drawInfo.shaderProgram = m_program;
	for (size_t i = 0; i < numGameObject; i++)
	{
		SetUniform(m_modelMatrixId, gameObject[i]->modelMat);
		gameObject[i]->model.tDraw(drawInfo);
	}
}
//=============================================================================