#include "pch.h"

#include "VulkanEngine.h"

VulkanEngine::~VulkanEngine()
{
	// wait for the device to finish all operations before destroying it
	m_Init->vkDeviceWaitIdle(m_Init.device);
	VulkanRenderer::CleanUp(m_Init, m_RenderData);
}

void VulkanEngine::Initialize(const char* windowName, int width, int height)
{
	m_Init       = VulkanInit::InitVulkan(1920, 1080, "UIEngine");
	m_RenderData = VulkanRenderer::SetupRenderData(m_Init);

	auto mesh = VulkanRenderer::LoadModel("../Resources/Meshes/test.obj");
	mesh.texturePath = "../Resources/Textures/test.png";
	mesh.modelMatrix = glm::mat4(1.0f);
	mesh.modelMatrix = glm::translate(mesh.modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	mesh.modelMatrix = glm::rotate(mesh.modelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mesh.modelMatrix = glm::scale(mesh.modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
	VulkanRenderer::SetUpMeshBuffers(m_Init, m_RenderData, mesh);
	VulkanRenderer::SetUpTexture(m_Init, m_RenderData, mesh);
	VulkanRenderer::SetUpMeshDescriptorInfo(m_Init, mesh);
	VulkanRenderer::SetUpMeshDescriptorSets(m_Init, mesh);
	VulkanRenderer::SetUpPipelineLayout(m_Init, m_RenderData, mesh);
	VulkanRenderer::RecordCommandBuffers(m_Init, m_RenderData, mesh);
	VulkanRenderer::AddMesh(mesh);
	
	// Output all the mesh details
	std::cout << "Mesh Details:" << std::endl;
	std::cout << "Vertices: " << mesh.vertices.size() << std::endl;
	std::cout << "Indices: " << mesh.indices.size() << std::endl;
	std::cout << "Directory: " << mesh.directory << std::endl;
}

float VulkanEngine::Update()
{
	glfwPollEvents();

	if (glfwGetKey(m_Init.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_Init.window, true);
	}

	// Test if wants to close
	if (glfwWindowShouldClose(m_Init.window))
	{
		m_close = true;
	}

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
