#include "pch.h"

//#include "TestingScene.h"
#include <Engine.h>

int main()
{
	// Memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	UIEngine::Engine engine = UIEngine::Engine();
	engine.Init("UIEngine", 1280, 720);

	//SceneStateMachine stateMachine = engine.GetStateMachine();

	//TestingScene testScene("Testing Scene");
	//stateMachine.AddScene(&testScene);
	//stateMachine.SetCurrentScene(&testScene);

	while (!engine.GetClose())
	{
		float dt = engine.Update();

		// If dt is -float max then don't continue
		if (dt == -FLT_MAX)
			break;

		engine.Render();
	}
}