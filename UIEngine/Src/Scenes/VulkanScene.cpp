#include "pch.h"

#include "VulkanScene.h"

#include <iostream>

#include "Engine/InputManager.h"
#include "Engine/VulkanAPI/Types/Mesh.h"

void VulkanScene::Enter()
{
	// Create the camera
	float fov         = 60;
	float aspectRatio = 16.0f / 9.0f;
	float nearPlane   = 0.1f;
	float farPlane    = 1000.0f;
	m_Camera.setProjectionMatrix(fov, aspectRatio, nearPlane, farPlane);

	m_DescriptorManager = std::make_shared<VulkanWrapper::DescriptorManager>(m_API.device);
	m_API.SetDescriptorManager(m_DescriptorManager);

	// Create the knot mesh
	m_KnotMesh = std::make_shared<VulkanWrapper::Mesh>(m_API.device, m_API.deviceQueue.m_PhysicalDevice, m_DescriptorManager.get());
	m_KnotMesh->LoadModel(m_API, "../Resources/Meshes/knot.obj");

	// Create the Cube mesh
	m_CubeMesh = std::make_shared<VulkanWrapper::Mesh>(m_API.device, m_API.deviceQueue.m_PhysicalDevice, m_DescriptorManager.get());
	m_CubeMesh->LoadModel(m_API, "../Resources/Meshes/cube.obj");

	// Create the graphics pipeline
	m_API.CreateGraphicsPipeline("../Resources/Shaders/compiled/vulkan_vert.spv", "../Resources/Shaders/compiled/vulkan_frag.spv", m_KnotMesh->GetAllDescriptorSetLayouts());

	m_Renderer.SetCamera(m_Camera);

	m_PointLight.color = glm::vec3(0.2f, 1.0f, 0.3f);
	m_PointLight.position   = glm::vec3(0.0f, 0.0f, 0.0f);
	m_PointLight.intensity = 1.0f;
	m_Renderer.AddPointLight(m_PointLight);
}

void VulkanScene::Exit()
{
	m_KnotMesh->Destroy();
	m_CubeMesh->Destroy();
}

void VulkanScene::Update(float deltaTime)
{
	// basic camera movement
	float cameraSpeed = 2.5f * deltaTime;
	if (InputManager::GetInstance().IsKeyHeld(Input::Keyboard::LEFT_SHIFT))
	{
		cameraSpeed *= 2.0f;
	}

	std::pair<float, float> mousePos = InputManager::GetInstance().GetMousePosition();

	if (InputManager::GetInstance().IsMouseButtonPressed(Input::Mouse::RIGHT))
	{
		float xoffset = mousePos.first - m_prevMousePos.first;
		float yoffset = m_prevMousePos.second - mousePos.second; // reversed since y-coordinates go from bottom to top

		m_Camera.Rotate(xoffset, -yoffset);
	}

	if (InputManager::GetInstance().IsKeyHeld('W'))
	{
		m_Camera.MoveForward(cameraSpeed);
	}
	if (InputManager::GetInstance().IsKeyHeld('A'))
	{
		m_Camera.MoveLeft(cameraSpeed);
	}
	if (InputManager::GetInstance().IsKeyHeld('S'))
	{
		m_Camera.MoveBackward(cameraSpeed);
	}
	if (InputManager::GetInstance().IsKeyHeld('D'))
	{
		m_Camera.MoveRight(cameraSpeed);
	}

	m_prevMousePos = mousePos;
}

void VulkanScene::Draw()
{
	glm::mat4 model = glm::mat4(1.0f);
	model           = glm::scale(model, glm::vec3(0.2f));
	m_Renderer.AddMesh(*m_KnotMesh, model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
	m_Renderer.AddMesh(*m_CubeMesh, model);
}