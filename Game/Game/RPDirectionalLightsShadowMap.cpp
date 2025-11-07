#include "stdafx.h"
#include "RPDirectionalLightsShadowMap.h"
#include "GameScene.h"
#include "NanoIO.h"
#include "NanoLog.h"
660
Riemann
//=============================================================================
bool RPDirectionalLightsShadowMap::Init(ShadowQuality shadowQuality)
{
	m_shadowQuality = shadowQuality;
	m_bias = 0.0005f;
	m_orthoDimension = 10.0f;
	m_orthoProjection = glm::ortho(-m_orthoDimension, m_orthoDimension, -m_orthoDimension, m_orthoDimension, 0.1f, 100.0f);

	if (!initProgram())
		return false;

	if (!initFBO())
		return false;

	return true;
}
//=============================================================================
void RPDirectionalLightsShadowMap::Close()
{
	if (m_program) 
		glDeleteProgram(m_program);

	for (size_t i = 0; i < m_depthFBO.size(); i++)
	{
		m_depthFBO[i].Destroy();
	}
}
//=============================================================================
void RPDirectionalLightsShadowMap::Draw(const GameWorldData& worldData)
{
	if (m_shadowQuality == ShadowQuality::Off) return;
	if (worldData.numDirLights == 0) return;

	glEnable(GL_DEPTH_TEST);
	glUseProgram(m_program);
	glViewport(0, 0, static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));

	glm::mat4 lightView;
	glm::mat4 vpMatrix;
	for (size_t i = 0; i < worldData.numDirLights; i++)
	{
		if (worldData.numDirLights >= m_depthFBO.size())
		{
			Warning("Num Dir Light bigger num");
			break;
		}

		const auto& light = worldData.dirLights[i];

		m_depthFBO[i].Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		constexpr glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::normalize(glm::cross(light->direction, worldUp));
		glm::vec3 up = glm::normalize(glm::cross(right, light->direction));

		lightView = glm::lookAt(light->position, light->position + light->direction, up);

		vpMatrix = m_orthoProjection * lightView;
		drawScene(vpMatrix, worldData);
	}
}
//=============================================================================
void RPDirectionalLightsShadowMap::SetShadowQuality(ShadowQuality quality)
{
	if (m_shadowQuality == quality) return;

	m_shadowQuality = quality;
	if (m_shadowQuality != ShadowQuality::Off)
	{
		for (size_t i = 0; i < m_depthFBO.size(); i++)
			m_depthFBO[i].Resize(static_cast<int>(m_shadowQuality), static_cast<int>(m_shadowQuality));
	}
}
//=============================================================================
void RPDirectionalLightsShadowMap::drawScene(const glm::mat4& transformMat, const GameWorldData& worldData)
{
	for (size_t i = 0; i < worldData.numGameObject; i++)
	{
		if (!worldData.gameObjects[i] || !worldData.gameObjects[i]->visible)
			continue;

		SetUniform(m_mvpMatrixId, transformMat * worldData.gameObjects[i]->modelMat);

		const auto& meshes = worldData.gameObjects[i]->model.GetMeshes();
		for (const auto& mesh : meshes)
		{
			const auto& material = mesh.GetPbrMaterial();
			bool hasAlbedoMap = false;
			GLuint albedoTex = 0;
			if (material)
			{
				hasAlbedoMap = material->albedoTexture.id > 0;
				albedoTex = material->albedoTexture.id;
			}

			SetUniform(m_hasAlbedoMapId, hasAlbedoMap);
			BindTexture2D(0, albedoTex);

			mesh.Draw(GL_TRIANGLES);
		}
	}
}
//=============================================================================
bool RPDirectionalLightsShadowMap::initProgram()
{
	m_program = LoadShaderProgram("data/shaders/shadowMapping/vertex.glsl", "data/shaders/shadowMapping/fragment.glsl");
	if (!m_program)
	{
		Fatal("Scene Shadow Mapping Shader failed!");
		return false;
	}
	glUseProgram(m_program);

	int albedoTextureId = GetUniformLocation(m_program, "albedoTexture");
	assert(albedoTextureId > -1);
	m_hasAlbedoMapId = GetUniformLocation(m_program, "hasAlbedoMap");
	assert(m_hasAlbedoMapId > -1);
	m_mvpMatrixId = GetUniformLocation(m_program, "mvpMatrix");
	assert(m_mvpMatrixId > -1);

	SetUniform(albedoTextureId, 0);

	glUseProgram(0); // TODO: возможно вернуть прошлую

	return true;
}
//=============================================================================
bool RPDirectionalLightsShadowMap::initFBO()
{
	FramebufferInfo depthFboInfo;
	depthFboInfo.depthAttachment = DepthAttachment{ .type = AttachmentType::Texture };
	depthFboInfo.width = static_cast<int>(m_shadowQuality);
	depthFboInfo.height = static_cast<int>(m_shadowQuality);
	for (size_t i = 0; i < m_depthFBO.size(); i++)
	{
		if (!m_depthFBO[i].Create(depthFboInfo))
			return false;
	}

	return true;
}
//=============================================================================