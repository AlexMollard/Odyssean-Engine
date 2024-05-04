#pragma once
#include "Node.h"
#include "glm/gtx/transform.hpp"
#include <vector>

namespace node
{
// This is a component class. It is used to store data about the transform and hierarchy of an entity.
// The hierarchy systems will be using the built-in flecs hierarchy system.

class Transform : Node
{
public:
	// Default constructor
	Transform() = default;

	// Constructor
	Transform(const glm::vec3& position, float rotation, const glm::vec3& scale) : m_Position(position), m_Rotation(rotation), m_Scale(scale) {}

	// Destructor
	~Transform() = default;

	// Getters
	const glm::vec2& GetPosition() const
	{
		return m_Position;
	}
	float GetRotation() const
	{
		return m_Rotation;
	}
	const glm::vec2& GetScale() const
	{
		return m_Scale;
	}

	// Setters
	void SetPosition(const glm::vec2& position)
	{
		m_Position = position;
	}
	void SetRotation(float rotation)
	{
		m_Rotation = rotation;
	}
	void SetScale(const glm::vec2& scale)
	{
		m_Scale = scale;
	}

	// Inspector
	void Inspector() override;

	glm::vec2 m_Position = glm::vec3(0.0f);
	float m_Rotation     = 0.0f;
	glm::vec2 m_Scale    = glm::vec3(1.0f);
};
} // namespace node