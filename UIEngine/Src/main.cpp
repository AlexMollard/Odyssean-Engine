#include "pch.h"

#include "Engine/MemPlumber/memplumber.h"

#include "Scenes/VulkanScene.h"

#include "Engine/Engine.h"
#include "Engine/SceneStateMachine.h"

int main()
{
	// start collecting mem allocations info
	MemPlumber::start();

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

	// run memory leak test in verbose mode
	size_t memLeakCount;
	uint64_t memLeakSize;
	MemPlumber::memLeakCheck(memLeakCount, memLeakSize, true);

	// print memory leak results
	printf("Number of leaked objects: %d\nTotal amount of memory leaked: %d[bytes]\n", (int)memLeakCount, (int)memLeakSize);

	MemPlumber::staticMemCheck(memLeakCount, memLeakSize, true);

	// print memory leak results
	printf("Number of leaked static objects: %d\nTotal amount of memory leaked: %d[bytes]\n", (int)memLeakCount, (int)memLeakSize);
}