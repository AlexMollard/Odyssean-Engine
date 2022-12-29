#pragma once
#include "glm/gtx/transform.hpp"
#include <vector>

namespace components
{
// This is a component class. It is used to store data about the transform and hierarchy of an entity.
// The hierarchy systems will be using the built-in flecs hierarchy system.

class Transform
{
public:
	// Default constructor
	Transform() = default;

	// Constructor
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : m_Position(position), m_Rotation(rotation), m_Scale(scale) {}

	// Destructor
	~Transform() = default;

	// Getters
	const glm::vec2& GetPosition() const { return m_Position; }
	const glm::vec2& GetRotation() const { return m_Rotation; }
	const glm::vec2& GetScale() const { return m_Scale; }

	// Setters
	void SetPosition(const glm::vec2& position) { m_Position = position; }
	void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
	void SetScale(const glm::vec2& scale) { m_Scale = scale; }

	glm::vec2 m_Position = glm::vec3(0.0f);
	glm::vec3 m_Rotation = glm::vec3(0.0f);
	glm::vec2 m_Scale    = glm::vec3(1.0f);

	// Get the model matrix of the transform
	glm::mat4 GetModelMatrix() const
	{
		glm::mat4 model = glm::mat4(1.0f);
		model           = glm::translate(model, glm::vec3(m_Position, 1.0f));
		model           = glm::rotate(model, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model           = glm::rotate(model, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model           = glm::rotate(model, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model           = glm::scale(model, glm::vec3(m_Scale, 1.0f));
		return model;
	}
};
} // namespace components