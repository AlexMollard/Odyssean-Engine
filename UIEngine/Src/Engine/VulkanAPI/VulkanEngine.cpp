#include "pch.h"

#include "VulkanEngine.h"

#include "ImGuiVulkan.h"

VulkanEngine::~VulkanEngine()
{
	// wait for the device to finish all operations before destroying it
	m_Init->vkDeviceWaitIdle(m_Init.device);

	VulkanRenderer::CleanUp(m_Init, m_RenderData);
}

void VulkanEngine::Initialize(const char* windowName, int width, int height)
{
	m_Init            = VulkanInit::InitVulkan(1920, 1080, "UIEngine");
	m_RenderData      = VulkanRenderer::SetupRenderData(m_Init);
	vk::Device device = m_Init.device.device;

	m_Mesh             = VulkanRenderer::LoadModel("../Resources/Meshes/cube.obj");
	m_Mesh.texturePath = "../Resources/Images/debug.png";
	m_Mesh.modelMatrix = glm::mat4(1.0f);
	m_Mesh.modelMatrix = glm::translate(m_Mesh.modelMatrix, glm::vec3(0.0f, 0.0f, -1.0f));
	m_Mesh.modelMatrix = glm::rotate(m_Mesh.modelMatrix, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	VulkanRenderer::SetUpMeshBuffers(m_Init, m_RenderData, m_Mesh);
	VulkanRenderer::SetUpTexture(m_Init, m_RenderData, m_Mesh);
	VulkanRenderer::SetUpMeshDescriptorInfo(m_Init, m_Mesh);
	VulkanRenderer::SetUpMeshDescriptorSets(m_Init, m_Mesh);
	m_Mesh.UpdateModelMatrix(device, m_Mesh.modelMatrix);
	VulkanRenderer::RecordCommandBuffers(m_Init, m_RenderData, m_Mesh);
	VulkanRenderer::AddMesh(m_Mesh);

	// Output all the m_Mesh details
	std::cout << "Mesh Details:" << std::endl;
	std::cout << "Vertices: " << m_Mesh.vertices.size() << std::endl;
	std::cout << "Indices: " << m_Mesh.indices.size() << std::endl;
	std::cout << "Directory: " << m_Mesh.directory << std::endl;

	ImGuiVulkan::SetUpImgui(m_Init, m_RenderData);
	ImGuiVulkan::RecordImguiCommandBuffers(m_Init, m_RenderData);
}

float VulkanEngine::Update()
{
	// Delta Time
	float currentFrame = glfwGetTime();
	m_DT               = currentFrame - m_LastFrame;
	m_LastFrame        = currentFrame;

	glfwPollEvents();

	if (glfwGetKey(m_Init.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(m_Init.window, true); }

	// Test if wants to close
	if (glfwWindowShouldClose(m_Init.window)) { m_close = true; }

	// Rotate the m_Mesh
	vk::Device device  = m_Init.device.device;
	m_Mesh.modelMatrix = glm::rotate(m_Mesh.modelMatrix, glm::radians(m_DT * 10.0f), glm::vec3(1.0f, 0.0f, 0.8f));
	m_Mesh.UpdateModelMatrix(device, m_Mesh.modelMatrix);

	ImGuiVulkan::UpdateImgui(m_Init, m_RenderData);

	return 0.0f;
}

void VulkanEngine::Render()
{
	int res = VulkanRenderer::DrawFrame(m_Init, m_RenderData);

	if (res != 0)
	{
		std::cout << "failed to draw frame \n";
		return;
	}
}