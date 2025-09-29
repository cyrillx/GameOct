#include "stdafx.h"
#include "GameApp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3.h"
#include "NanoRender.h"
#include "NanoScene.h"
#include "GridAxis.h"
#include "Scene.h"
//=============================================================================
namespace
{
	Scene scene;

	Camera camera;

	Entity modelPlane;
	Entity modelBox;
	Entity modelSphere;
	Entity modelTest;
	Entity modelTest2;
}
//=============================================================================
void ExampleApp002()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		scene.Init();
		camera.SetPosition(glm::vec3(0.0f, 0.5f, 4.5f));
		scene.SetGridAxis(22);

		modelPlane.model.Create(GeometryGenerator::CreatePlane(100, 100, 100, 100));
		modelBox.model.Create(GeometryGenerator::CreateBox());
		modelSphere.model.Create(GeometryGenerator::CreateSphere());
		modelTest.model.Load("data/models/tree.glb");
		
		modelTest2.model.Load("data/models/cottage/cottage_obj.obj");
		// TRS matrix
		modelTest2.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -3.0f));
		// rotate mat
		modelTest2.modelMat = glm::scale(modelTest2.modelMat, glm::vec3(0.2f));

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

			{
				if (input::IsKeyDown(GLFW_KEY_W)) camera.ProcessKeyboard(CameraForward, engine::GetDeltaTime());
				if (input::IsKeyDown(GLFW_KEY_S)) camera.ProcessKeyboard(CameraBackward, engine::GetDeltaTime());
				if (input::IsKeyDown(GLFW_KEY_A)) camera.ProcessKeyboard(CameraLeft, engine::GetDeltaTime());
				if (input::IsKeyDown(GLFW_KEY_D)) camera.ProcessKeyboard(CameraRight, engine::GetDeltaTime());

				if (input::IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT))
				{
					input::SetCursorVisible(false);
					camera.ProcessMouseMovement(input::GetCursorOffset().x, input::GetCursorOffset().y);
				}
				else if (input::IsMouseReleased(GLFW_MOUSE_BUTTON_RIGHT))
				{
					input::SetCursorVisible(true);
				}
			}

			scene.BindCamera(&camera);
			scene.BindEntity(&modelTest2);
			scene.Draw();

			engine::DrawFPS();

			engine::EndFrame();
		}
	}
	catch (const std::exception& exc)
	{
		puts(exc.what());
	}
	engine::Close();
}
//=============================================================================