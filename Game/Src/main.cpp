#include "pch.h"

#include "SceneStateMachine.h"
#include "TestingScene.h"
#include "VulkanScene.h"
#include <Engine.h>

int main()
{
	// Memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	UIEngine::Engine engine = UIEngine::Engine();
	engine.Init("UIEngine", 1920, 1080, GraphicsAPI::OpenGL);

	SceneStateMachine stateMachine;
	TestingScene testScene("Testing Scene");
	VulkanScene vulkanScene("Vulkan Scene");

	stateMachine.AddScene(&testScene);
	stateMachine.AddScene(&vulkanScene);
	stateMachine.SetCurrentScene(&vulkanScene);

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
}