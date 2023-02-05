#include "pch.h"

#include "VulkanScene.h"

#include "Engine/VulkanAPI/Types/Mesh.h"

void VulkanScene::Enter()
{
	// Create the camera
	float fov = 90;
	float aspectRatio = 16.0f / 9.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	m_Camera.setProjectionMatrix(fov, aspectRatio, nearPlane, farPlane);

	m_DescriptorManager = std::make_shared<VulkanWrapper::DescriptorManager>(m_API.device);
	m_API.SetDescriptorManager(m_DescriptorManager);

	// Create the knot mesh
	m_KnotMesh = std::make_shared<VulkanWrapper::Mesh>(m_API.device, m_API.deviceQueue.m_PhysicalDevice, m_DescriptorManager.get());
	m_KnotMesh->LoadModel(m_API, "../Resources/Meshes/knot.obj");

	// Create the graphics pipeline
	m_API.CreateGraphicsPipeline("../Resources/Shaders/compiled/vulkan_vert.spv", "../Resources/Shaders/compiled/vulkan_frag.spv", m_KnotMesh->GetAllDescriptorSetLayouts());

	m_Renderer.SetCamera(m_Camera);
}

void VulkanScene::Exit()
{
	m_KnotMesh->Destroy();
}

void VulkanScene::Update(float deltaTime)
{
	// basic camera movement
	m_Camera.Move(glm::vec3(0.0f, 0.0f, -1.0f), deltaTime);
}

void VulkanScene::Draw()
{
	m_Renderer.AddMesh(*m_KnotMesh, glm::mat4(1.0f));
}