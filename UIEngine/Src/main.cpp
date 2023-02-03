#include "Scenes/VulkanScene.h"
#include <crtdbg.h>

#include "Engine/Engine.h"
#include "Engine/SceneStateMachine.h"

int main()
{
	//Turn on debugging for memory leaks. This is automatically turned off when the build is Release.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	GraphicsAPI graphicsAPI = GraphicsAPI::Vulkan;

	UIEngine::Engine engine = UIEngine::Engine();
	engine.Init("UIEngine", 1920, 1080, graphicsAPI);

	auto& sceneStateMachine = engine.GetSceneStateMachine();

	VulkanScene scene("TestingScene");

	sceneStateMachine.AddScene(&scene);
	sceneStateMachine.SetCurrentScene(&scene);

	while (!engine.GetClose())
	{
		float dt = engine.Update();

		// If dt is -float max then don't continue
		if (dt == -FLT_MAX) break;

		engine.Render();
	}

	scene.Exit();
}