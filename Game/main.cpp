#include "pch.h"

#include "SceneStateMachine.h"
#include "TestingScene.h"
#include <Engine.h>

int main()
{
	// Memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	UIEngine::Engine engine = UIEngine::Engine();
	engine.Init("UIEngine", 1920,1080);

	SceneStateMachine stateMachine;

	TestingScene testScene("Testing Scene");
	stateMachine.AddScene(&testScene);
	stateMachine.SetCurrentScene(&testScene);

	Window* window = static_cast<Window*>(engine.GetWindow());

	while (!engine.GetClose())
	{
		float dt = engine.Update();

		// If dt is -float max then don't continue
		if (dt == -FLT_MAX)
			break;

		stateMachine.update(dt);

		stateMachine.render(*window);
		engine.Render();
	}
}