#pragma once
#include <vector>

class Scene;
class Window;
class Services;
class SceneStateMachine
{
public:
	explicit SceneStateMachine(const Services* services);
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
	void update(float deltaTime);

	// Render the current state
	void render(Window& window);

	// Destroy all scenes.
	void Destroy();

private:
	Scene* currentScene = nullptr;
	const Services* services = nullptr; 
	std::vector<Scene*> scenes;
};
