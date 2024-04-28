#include "pch.h"

#include "Scenes/VulkanScene.h"

#include "Engine/Engine.h"
#include "Engine/SceneStateMachine.h"

int main()
{
	GraphicsAPI graphicsAPI = GraphicsAPI::Vulkan;

	auto engine = UIEngine::Engine();
	engine.Init("UIEngine", 1920, 1080, graphicsAPI);

	auto& sceneStateMachine = engine.GetSceneStateMachine();

	VulkanScene scene("TestingScene");

	sceneStateMachine.AddScene(&scene);
	sceneStateMachine.SetCurrentScene(&scene);

	while (!engine.GetClose())
	{
		// If dt is -float max then don't continue
		if (engine.Update() == -FLT_MAX)
			break;

		engine.Render();
	}

	scene.Exit();
}
