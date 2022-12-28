#pragma once
#include <string>

class Scene
{
public:
	// Called when scene initially created. Called once.
	virtual void OnCreate() = 0;

	// Called when scene destroyed. Called at most once (if a scene
	// is not removed from the game, this will never be called).
	virtual void OnDestroy() = 0;

	// Called whenever a scene is transitioned into. Can be
	// called many times in a typical game cycle.
	virtual void Enter(){ /* Override This */ };

	// Called whenever a transition out of a scene occurs.
	// Can be called many times in a typical game cycle.
	virtual void Exit(){ /* Override This */ };

	// The below functions can be overridden as necessary in our scenes.
	virtual void ProcessInput(){ /* Override This */ };
	virtual void Update(float deltaTime){ /* Override This */ };
	virtual void LateUpdate(float deltaTime){ /* Override This */ };
	virtual void Draw(){ /* Override This */ };

	// virtual destructor
	virtual ~Scene() = default;
};
