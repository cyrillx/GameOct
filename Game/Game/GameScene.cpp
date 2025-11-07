#include "stdafx.h"
#include "GameScene.h"
#include "NanoWindow.h"
#include "NanoIO.h"
#include "NanoLog.h"
//=============================================================================
bool GameScene::Init()
{
	m_data.Init();

	const auto wndWidth = window::GetWidth();
	const auto wndHeight = window::GetHeight();

	if (!m_rpDirShadowMap.Init(ShadowQuality::High))
		return false;
	if (!m_rpGeometry.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpSSAO.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpSSAOBlur.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpBlinnPhong.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpMainScene.Init(wndWidth, wndHeight))
		return false;
	if (!m_rpComposite.Init(wndWidth, wndHeight))
		return false;
		
	return true;
}
//=============================================================================
void GameScene::Close()
{
	m_rpMainScene.Close();
	m_rpBlinnPhong.Close();
	m_rpComposite.Close();
	m_rpSSAOBlur.Close();
	m_rpSSAO.Close();
	m_rpGeometry.Close();
	m_rpDirShadowMap.Close();
}
//=============================================================================
void GameScene::BindCamera(Camera* camera)
{
	m_data.camera = camera;
}
//=============================================================================
void GameScene::BindGameObject(GameObject* go)
{
	if (m_data.numGameObject >= m_data.gameObjects.size())
		m_data.gameObjects.push_back(go);
	else
		m_data.gameObjects[m_data.numGameObject] = go;

	m_data.numGameObject++;
}
//=============================================================================
void GameScene::BindLight(DirectionalLight* ent)
{
	if (m_data.numDirLights >= MaxDirectionalLight)
	{
		Error("Max dir light");
		return;
	}
	m_data.dirLights[m_data.numDirLights] = ent;
	m_data.numDirLights++;
}
//=============================================================================
void GameScene::Draw()
{
	if (!m_data.camera)
	{
		Warning("Not active camera");
		return;
	}
	if (!m_data.numGameObject)
	{
		Warning("Not active entities");
		return;
	}

	beginDraw();
	draw();
	endDraw();
}
//=============================================================================
void GameScene::beginDraw()
{
	const auto wndWidth = window::GetWidth();
	const auto wndHeight = window::GetHeight();

	m_rpGeometry.Resize(wndWidth, wndHeight);
	m_rpSSAO.Resize(wndWidth, wndHeight);
	m_rpSSAOBlur.Resize(wndWidth, wndHeight);
	m_rpBlinnPhong.Resize(wndWidth, wndHeight);
	m_rpMainScene.Resize(wndWidth, wndHeight);
	m_rpComposite.Resize(wndWidth, wndHeight);
}
//=============================================================================
void GameScene::draw()
{
	//================================================================================
	// 1.) Render Pass: render depth of scene to texture (from light's perspective)
	//		Set state: glEnable(GL_DEPTH_TEST);
	m_rpDirShadowMap.Draw(m_data);

	if (EnableSSAO)
	{
		//================================================================================
		// 2.1 Render Pass: geometry
		m_rpGeometry.Draw(m_data.gameObjects, m_data.numGameObject, m_data.camera);

		//================================================================================
		// 2.2 Render Pass: SSAO
		//		Set state: glDisable(GL_DEPTH_TEST);
		m_rpSSAO.Draw(&m_rpGeometry.GetFBO());

		//================================================================================
		// 2.3 Render Pass: SSAO Blur
		m_rpSSAOBlur.Draw(&m_rpSSAO.GetFBO());
	}
	
	//================================================================================
	// 3 Render Pass: main scenes
	//		Set state: glEnable(GL_DEPTH_TEST);
	//m_rpBlinnPhong.Draw(m_rpDirShadowMap, m_dirLights, m_numDirLights, m_gameObjects, m_numGO, m_camera);
	m_rpMainScene.Draw(m_rpDirShadowMap, m_data.gameObjects, m_data.numGameObject, m_data.camera);
	
	//================================================================================
	// 4 Render Pass: post frame
	//		Set state: glDisable(GL_DEPTH_TEST);
	//m_rpComposite.Draw(&m_rpBlinnPhong.GetFBO(), EnableSSAO ? &m_rpSSAOBlur.GetFBO() : nullptr);
	m_rpComposite.Draw(&m_rpMainScene.GetFBO(), EnableSSAO ? &m_rpSSAOBlur.GetFBO() : nullptr);

	//================================================================================
	// 5 Render Pass: blitting main fbo
	blittingToScreen(m_rpComposite.GetFBOId(), m_rpComposite.GetWidth(), m_rpComposite.GetHeight());
}
//=============================================================================
void GameScene::endDraw()
{
	m_data.ResetFrame();
}
//=============================================================================
void GameScene::blittingToScreen(GLuint fbo, uint16_t srcWidth, uint16_t srcHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0, 0, srcWidth, srcHeight,
		0, 0, window::GetWidth(), window::GetHeight(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
//=============================================================================