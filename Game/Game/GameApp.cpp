#include "stdafx.h"
#include "GameApp.h"
#include "CameraOld.h"
#include "ModelTemp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoOpenGL3.h"
#include "NanoIO.h"
// https://github.com/Keypekss/OpenGLTechDemo
// https://github.com/vcoda/aggregated-graphics-samples
// https://github.com/Mikepicker/opengl-sandbox
// https://github.com/cforfang/opengl-shadowmapping
// https://github.com/uobirek/opengl-3D-renderer
// https://github.com/pulkitjuneja/GlEn
//=============================================================================
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;
//=============================================================================
void GameAppRun()
{
	try
	{
		if (!window::Init(SCR_WIDTH, SCR_HEIGHT, "Game"))
			return;

		GLState state;
		state.depthState.enable = true;
		state.blendState.enable = true;
		state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

		while (!window::WindowShouldClose())
		{
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// render
			BindState(state);
			glClearColor(0.1f, 0.1f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			window::Swap();
		}
	}
	catch (const std::exception& exc)
	{
		puts(exc.what());
	}
	window::Close();
}
//=============================================================================