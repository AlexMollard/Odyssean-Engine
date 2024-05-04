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

	void Move(const glm::vec3& direction, float deltaTime)
	{
		m_Position += direction * deltaTime;
	}

	void Rotate(float xoffset, float yoffset)
	{
		xoffset *= m_sensitivity;
		yoffset *= m_sensitivity;

		m_Yaw += xoffset;
		m_Pitch += yoffset;

		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);
	}

	void MoveForward(float cameraSpeed)
	{
		m_Position += m_Front * cameraSpeed;
	}

	void MoveLeft(float cameraSpeed)
	{
		m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * cameraSpeed;
	}

	void MoveBackward(float cameraSpeed)
	{
		m_Position -= m_Front * cameraSpeed;
	}

	void MoveRight(float cameraSpeed)
	{
		m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * cameraSpeed;
	}

private:
	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 m_Front    = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_Up       = glm::vec3(0.0f, 1.0f, 0.0f);

	float m_Yaw         = -90.0f;
	float m_Pitch       = 0.0f;
	float m_sensitivity = 0.1f;

	glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
};
} // namespace node
