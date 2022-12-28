#include "pch.h"

#include "SceneStateMachine.h"
#include "TestingScene.h"
#include "VulkanScene.h"
#include <Engine.h>

int main()
{
	// Memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	GraphicsAPI graphicsAPI = GraphicsAPI::Vulkan;

	UIEngine::Engine engine = UIEngine::Engine();
	engine.Init("UIEngine", 1920, 1080, graphicsAPI);

	SceneStateMachine stateMachine;
	Scene* scene = nullptr;

	if (graphicsAPI == GraphicsAPI::OpenGL)
		scene = new TestingScene("TestingScene");
	else if (graphicsAPI == GraphicsAPI::Vulkan)
		scene = new VulkanScene("Vulkan Scene");

	stateMachine.AddScene(scene);
	stateMachine.SetCurrentScene(scene);

	while (!engine.GetClose())
	{
		float dt = engine.Update();

		// If dt is -float max then don't continue
		if (dt == -FLT_MAX)
			break;

		stateMachine.update(dt);

		stateMachine.render();
		engine.Render();
	}

	scene->Exit();
	delete scene;
}