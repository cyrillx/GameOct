#pragma once

#include "GameWorldData.h"
#include "RenderPass2.h"
#include "RenderPassFinal.h"

struct GameModel final
{
	Model     model;
	glm::mat4 modelMat{ glm::mat4(1.0f) };
	bool      visible{ true };
};

class GameScene final
{
public:
	bool Init();
	void Close();
	void Draw();

	void Bind(Camera* camera);
	void Bind(GameModel* go);

private:
	void beginDraw();
	void draw();
	void endDraw();

	void blittingToScreen(GLuint fbo, uint16_t srcWidth, uint16_t srcHeight);

	GameWorldData   m_data;
	RenderPass2     m_mainScene;
	RenderPassFinal m_composite;
};