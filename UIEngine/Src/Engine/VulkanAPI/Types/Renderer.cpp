#include "Renderer.h"

#include "Mesh.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

vulkan::Renderer::~Renderer()
{
	//Destroy();
}

void vulkan::Renderer::Init(vulkan::API* api)
{
	m_API = api;

	// Create the swap chain
	m_API->CreateSwapChain();

	// Create the render pass
	m_API->CreateRenderPass();

	// Create the frame buffers
	m_API->CreateFrameBuffers();

	// Create the sync objects
	m_API->syncObjectContainer.init(&m_API->deviceQueue.m_Device, m_API->swapchainInfo.m_ImageCount);

	// Create the command buffers
	m_API->CreateCommandBuffers();

	m_Mesh              = new vulkan::Mesh();
	m_Mesh->texturePath = "../Resources/Images/debug.png";
	m_Mesh->LoadModel("../Resources/Meshes/cube.obj");
	m_Mesh->Create(*api);

	m_LightMesh              = new vulkan::Mesh();
	m_LightMesh->LoadModel("../Resources/Meshes/cube.obj");
	m_LightMesh->Create(*api);

	// Create the graphics pipeline
	m_API->CreateGraphicsPipeline("../Resources/Shaders/compiled/vulkan_vert.spv", "../Resources/Shaders/compiled/vulkan_frag.spv", m_Mesh->descriptorSetLayout);
}

void vulkan::Renderer::Destroy()
{
	m_Mesh->Destroy(m_API->deviceQueue.m_Device);
	delete m_Mesh;

	m_LightMesh->Destroy(m_API->deviceQueue.m_Device);
	delete m_LightMesh;
}

void vulkan::Renderer::recreateSwapChain()
{
	// Wait for the device to be idle
	m_API->deviceQueue.wait();

	// Recreate the swap chain
	m_API->CreateSwapChain();

	// Recreate the render pass
	m_API->CreateRenderPass();

	// Recreate the frame buffers
	m_API->CreateFrameBuffers();

	// Recreate the command buffers
	m_API->CreateCommandBuffers();
}

void vulkan::Renderer::BeginFrame()
{
	// Get the next image
	vk::Result result = m_API->swapchainInfo.GetNextImage(m_API->deviceQueue.m_Device, m_API->syncObjectContainer.getImageAvailableSemaphore());

	// Check the result of getting the next image
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		// Recreate the swapchain and all the dependent resources
		recreateSwapChain();
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) { throw std::runtime_error("Failed to acquire next image: " + vk::to_string(result)); }

	// Set the current frame
	m_API->syncObjectContainer.setCurrentFrame(m_API->swapchainInfo.getCurrentFrameIndex());

	// We begin the command buffer
	m_API->commandBuffers[m_API->swapchainInfo.getCurrentFrameIndex()].Begin();

	// create view and projection matrices once
	static glm::mat4 view(1.0f);
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	static glm::mat4 proj(1.0f);
	proj = glm::perspective(glm::radians(90.0f), m_API->swapchainInfo.m_Extent.width / (float)m_API->swapchainInfo.m_Extent.height, 0.1f, 100.0f);

	// create time variable once
	static auto startTime   = std::chrono::high_resolution_clock::now();
	const auto  currentTime = std::chrono::high_resolution_clock::now();
	const float time        = std::chrono::duration<float>(currentTime - startTime).count();

	LightProperties light;
	light.lightPos   = glm::vec3(glm::sin(time) * 2.0f, 0.0f, glm::cos(time) * 2.0f);
	float r          = glm::sin(time * 2.0f) * 0.5f + 0.5f;
	float g          = glm::sin(time * 0.7f) * 0.5f + 0.5f;
	float b          = glm::sin(time * 1.3f) * 0.5f + 0.5f;
	light.lightColor = glm::vec4(r, g, b, 0.75f);

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	model = glm::rotate(model, glm::radians(time * 10), glm::vec3(1.0f, 1.0f, 0.0f));

	glm::mat4 lightModel(1.0f);
	lightModel = glm::translate(lightModel, light.lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(0.1f));

	glm::mat4 ubo      = proj * view * model;
	glm::mat4 uboLight = proj * view * lightModel;

	m_Mesh->UpdateLightProperties(m_API->deviceQueue.m_Device, light);
	m_Mesh->UpdateUBOMatrix(m_API->deviceQueue.m_Device, ubo);

	// Place light right above lightmodel
	light.lightPos = glm::vec3(lightModel[3][0], lightModel[3][1], lightModel[3][2]);

	m_LightMesh->UpdateLightProperties(m_API->deviceQueue.m_Device, light);
	m_LightMesh->UpdateUBOMatrix(m_API->deviceQueue.m_Device, uboLight);

	m_API->commandBuffers[m_API->swapchainInfo.getCurrentFrameIndex()].DoRenderPass(
		m_API->renderPassFrameBuffers.m_RenderPass, m_API->renderPassFrameBuffers.m_Framebuffers[m_API->swapchainInfo.getCurrentFrameIndex()], m_API->swapchainInfo.m_Extent, [&]() {
			m_LightMesh->AddToCommandBuffer(m_API->deviceQueue.m_Device, m_API->commandBuffers[m_API->swapchainInfo.getCurrentFrameIndex()], m_API->graphicsPipeline, m_API->graphicsPipelineLayout);

			m_Mesh->AddToCommandBuffer(m_API->deviceQueue.m_Device, m_API->commandBuffers[m_API->swapchainInfo.getCurrentFrameIndex()], m_API->graphicsPipeline, m_API->graphicsPipelineLayout);
		});
}

void vulkan::Renderer::EndFrame()
{
	vk::Device& device = m_API->deviceQueue.m_Device;

	// Get the command buffer and submit info
	vulkan::CommandBuffer& commandBuffer = m_API->commandBuffers[m_API->syncObjectContainer.getCurrentFrame()];

	vk::SubmitInfo submitInfo = m_API->syncObjectContainer.getSubmitInfo(commandBuffer.get());

	// Reset the fence associated with the current image
	m_API->syncObjectContainer.resetFences();

	// Submit the command buffer
	m_API->deviceQueue.GetQueue(QueueType::GRAPHICS).submit(submitInfo, m_API->syncObjectContainer.getInFlightFence());

	// Wait for the fence associated with the current image with a timeout of 1 second
	vk::Result result = m_API->syncObjectContainer.waitForFences();
	if (result == vk::Result::eTimeout)
	{
		std::cout << "Timeout waiting for fence" << std::endl;
		return;
	}
	if (result != vk::Result::eSuccess) { throw std::runtime_error("Failed to wait for fence: " + vk::to_string(result)); }

	// Present the image
	result = m_API->deviceQueue.Present(m_API->swapchainInfo.m_Swapchain, m_API->syncObjectContainer.getCurrentFrame(), m_API->syncObjectContainer.getRenderFinishedSemaphore());
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		// Recreate the swapchain and all the dependent resources
		recreateSwapChain();
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) { throw std::runtime_error("Failed to present image: " + vk::to_string(result)); }
}

void vulkan::Renderer::RenderUI()
{}
