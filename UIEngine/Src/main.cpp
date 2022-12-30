#include "pch.h"

#include "Scenes/TestingScene.h"
#include "Scenes/VulkanScene.h"
#include <crtdbg.h>

#include "Engine/Engine.h"
#include "Engine/SceneStateMachine.h"

int main()
{
	//Turn on debugging for memory leaks. This is automatically turned off when the build is Release.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	GraphicsAPI graphicsAPI = GraphicsAPI::OpenGL;

	UIEngine::Engine engine = UIEngine::Engine();
	engine.Init("UIEngine", 1920, 1080, graphicsAPI);

	SceneStateMachine stateMachine;
	Scene* scene = nullptr;

	const auto* renderer = static_cast<const BaseRenderer*>(engine.GetRenderer());

	// Create a new scene based on the graphics API and get renderer
	if (graphicsAPI == GraphicsAPI::OpenGL)
		scene = _NEW TestingScene("TestingScene");
	else if (graphicsAPI == GraphicsAPI::Vulkan)
		scene = _NEW VulkanScene("Vulkan Scene");

	stateMachine.AddScene(scene);
	stateMachine.SetCurrentScene(scene);

	while (!engine.GetClose())
	{
		float dt = engine.Update();

		// If dt is -float max then don't continue
		if (dt == -FLT_MAX)
			break;

		stateMachine.Update(dt);

		stateMachine.Render(*renderer);
		engine.Render();
	}

	scene->Exit();
	delete scene;
}