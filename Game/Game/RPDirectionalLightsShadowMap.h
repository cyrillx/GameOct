#pragma once

#include "Framebuffer.h"
#include "NanoScene.h"

enum class ShadowQuality 
{
	Off = 0,
	Tiny = 256,
	Small = 512,
	Med = 1024,
	High = 2048,
	Ultra = 4096,
	Mega = 8192
};

struct GameWorldData;

class RPDirectionalLightsShadowMap final
{
public:
	bool Init(ShadowQuality shadowQuality);
	void Close();

	void Draw(const GameWorldData& worldData);

	void SetShadowQuality(ShadowQuality quality);
	float GetBias() const { return m_bias; }

	const auto& GetDepthFBO() const { return m_depthFBO; }
	const auto& GetProjection() const { return m_orthoProjection; }

private:
	bool initProgram();
	bool initFBO();
	void drawScene(const glm::mat4& transformMat, const GameWorldData& worldData);

	GLuint        m_program{ 0 };
	int           m_mvpMatrixId{ -1 };
	int           m_hasAlbedoMapId{ -1 };

	ShadowQuality m_shadowQuality;
	float         m_bias;
	float         m_orthoDimension;
	glm::mat4     m_orthoProjection; // for directional lights

	std::array<Framebuffer, MaxDirectionalLight> m_depthFBO;
};