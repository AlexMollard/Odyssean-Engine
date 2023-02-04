#pragma once
#include "pch.h"
#include "BS_thread_pool.hpp"
#include "Nodes.h"
#include "flecs.h"
#include <thread>

// Create the singleton ECS class
class ECS
{
public:
	// Create the singleton instance
	static ECS* Instance()
	{
		if (s_Instance == nullptr) { s_Instance = _NEW ECS(); }
		return s_Instance;
	}

	// Initialize the ECS
	void Init();

	// Update the ECS
	void Update(const BS::thread_pool& pool);

	// Destroy the ECS
	void Destroy();

	ecs_ftime_t delta_time() const;

	// Create an entity
	flecs::entity CreateEntity();

	// Get the ECS world
	flecs::world* GetWorld()
	{
		return &m_World;
	}

	// Static Get the ECS world
	static flecs::world& GetWorldStatic()
	{
		return Instance()->m_World;
	}

	// Create a basic quad entity (quad, transform)
	static flecs::entity* CreateQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const char* name = nullptr);

	// Create a basic text entity (text, transform)
	static flecs::entity* CreateText(const std::string& text, const glm::vec3& position, const glm::vec4& color);

	static flecs::entity& GetRootEntity()
	{
		return Instance()->m_RootEntity;
	}

	// Gets a given nodes inspector function that is used to draw the node in the inspector
	static void GetNodeInspectorFunction(const flecs::entity& entity);

private:
	// Private constructor
	ECS() = default;

	// The singleton instance
	static ECS* s_Instance;

	// The ECS world
	flecs::world m_World;

	// Debug profiling stuff (Probably should be moved to a different class)
	// fps counter
	float m_FPS     = 0.0f;
	float m_FPSLow  = 0.0f;
	float m_FPSHigh = 0.0f;

	// Average fps over 1 second
	float m_FPSAverage = 0.0f;

	// Array of fps values over 1 second (for average)
	std::vector<float> m_FpsHistory;
	std::vector<float> m_AverageFpsHistory;

	float m_FPSTimer = 0.0f;

	// The root node of the ECS
	flecs::entity m_RootEntity;
};
