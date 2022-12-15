#pragma once
#include "Components.h"
#include "flecs.h"

// Create the singleton ECS class
class ECS
{
public:
	// Create the singleton instance
	static ECS* Instance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new ECS();
		}
		return s_Instance;
	}

	// Initialize the ECS
	void Init();

	// Update the ECS
	void Update();

	// Destroy the ECS
	void Destroy();

	// Create an entity
	flecs::entity CreateEntity();

	// Get the ECS world
	flecs::world* GetWorld() { return &m_World; }

private:
	// Private constructor
	ECS() = default;

	// The singleton instance
	static ECS* s_Instance;

	// The ECS world
	flecs::world m_World;
};
