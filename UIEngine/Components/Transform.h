#pragma once
#include "glm/gtx/transform.hpp"

namespace components
{
// This is a component class. It is used to store data about the transform of an entity.
class Transform
{
public:
	// Default constructor
	Transform() = default;

	// Constructor
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	: m_Position(position), m_Rotation(rotation), m_Scale(scale)
	{}

	// Destructor
	~Transform() = default;

	// Getters
	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetRotation() const { return m_Rotation; }
	const glm::vec3& GetScale() const { return m_Scale; }

	// Setters
	void SetPosition(const glm::vec3& position) { m_Position = position; }
	void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
	void SetScale(const glm::vec3& scale) { m_Scale = scale; }

private:
	glm::vec3 m_Position = glm::vec3(0.0f);
	glm::vec3 m_Rotation = glm::vec3(0.0f);
	glm::vec3 m_Scale    = glm::vec3(1.0f);
};
} // namespace components