#pragma once

class Camera;
struct GameObject;
struct DirectionalLight;

struct GameWorldData final // TODO;
{
	void Init()
	{
		gameObjects.reserve(10000);
		dirLights.resize(MaxDirectionalLight);
	}

	void ResetFrame()
	{
		camera = nullptr;
		numGameObject = 0;
		numDirLights = 0;
	}

	Camera*                        camera{ nullptr };
	std::vector<GameObject*>       gameObjects;
	size_t                         numGameObject{ 0 };
	std::vector<DirectionalLight*> dirLights;
	size_t                         numDirLights{ 0 };


};