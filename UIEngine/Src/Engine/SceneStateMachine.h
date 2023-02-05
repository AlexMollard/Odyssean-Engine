#pragma once
#include "BaseRenderer.h"
#include <vector>

class Scene;
class SceneStateMachine
{
public:
	SceneStateMachine()  = default;
	~SceneStateMachine() = default;

	// Add a scene to the state machine.
	void AddScene(Scene* scene);

	// Remove a scene from the state machine.
	void RemoveScene(Scene* scene);

	// Set the current scene.
	void SetCurrentScene(Scene* scene);

	// Get the current scene.
	Scene* GetCurrentScene() const;

	// Update the current state
	void Update(float deltaTime);

	// Render the current state
	void Render();

	// Destroy all scenes.
	void Destroy();

private:
	Scene*              currentScene = nullptr;
	std::vector<Scene*> scenes;
};
