#include "pch.h"

#include "SceneStateMachine.h"

#include "Scene.h"
#include "tracy/Tracy.hpp"

SceneStateMachine::SceneStateMachine(const Services* services) : services(services) {}

// Add a scene to the state machine.
void SceneStateMachine::AddScene(Scene* scene)
{
	// Check if the scene is already in the state machine.
	for (auto& s : scenes)
	{
		if (s == scene)
		{
			return;
		}
	}

	scenes.push_back(scene);
}

// Remove a scene from the state machine.
void SceneStateMachine::RemoveScene(Scene* scene)
{
	// Check if the scene is in the state machine.
	for (auto it = scenes.begin(); it != scenes.end(); ++it)
	{
		if (*it == scene)
		{
			scenes.erase(it);
			return;
		}
	}

	// If we get here, the scene was not in the state machine.
	S_WARN(false, "Scene not found in state machine.");
}

// Set the current scene.
void SceneStateMachine::SetCurrentScene(Scene* scene)
{
	if (currentScene != nullptr)
        currentScene->Exit();

	currentScene = scene;
	currentScene->Enter();
}

// Get the current scene.
Scene* SceneStateMachine::GetCurrentScene() const
{
	return currentScene;
}

void SceneStateMachine::update(float deltaTime) 
{
	ZoneScopedN("StateMachine Update");

	currentScene->Update(deltaTime, services);
}

void SceneStateMachine::render(Window& window) 
{
	ZoneScopedN("StateMachine Render");
	currentScene->Draw(window, services);
}

void SceneStateMachine::Destroy()
{
	for (auto& s : scenes)
	{
		s->OnDestroy();
		delete s;
	}

	scenes.clear();
}