#pragma once
#include "Node.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

namespace node
{
class Camera : public Node
{
public:
	Camera() = default;

	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	glm::mat4 getProjectionMatrix() const
	{
		return m_ProjectionMatrix;
	}

	void setProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	}

private:
	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 m_Front    = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_Up       = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

public:

	void Move(const glm::vec3& direction, float deltaTime)
	{
		m_Position += direction * deltaTime;
	}
};
} // namespace node
