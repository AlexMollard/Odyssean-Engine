#include "pch.h"

#include "VkRenderHelper.h"

#include "../Types/Mesh.h"
#include "../Types/VkContainer.h"
#include "Nodes/Camera.h"
#include <Engine/VulkanAPI/ImGuiVulkan.h>
#include <vector>

VkRenderHelper::VkRenderHelper()
{
	VulkanWrapper::VkContainer& m_API = VulkanWrapper::VkContainer::instance();

	// Create the swap chain
	m_API.CreateSwapChain();

	// Create the render pass
	m_API.CreateRenderPasses();

	// Create the frame buffers
	m_API.CreateFrameBuffers();

	// Create the sync objects
	m_API.syncObjectContainer.init(&m_API.deviceQueue.m_Device, m_API.swapchainInfo.m_ImageCount);

	// Create the command buffers
	m_API.CreateCommandBuffers();
}

void VkRenderHelper::DrawFrame()
{
	BeginFrame();
	EndFrame();
}

void VkRenderHelper::recreateSwapChain() const
{
	VulkanWrapper::VkContainer& m_API = VulkanWrapper::VkContainer::instance();

	// Wait for the device to be idle
	m_API.deviceQueue.wait();

	// Recreate the swap chain
	m_API.CreateSwapChain();

	// Recreate the render pass
	m_API.CreateRenderPasses();

	// Recreate the frame buffers
	m_API.CreateFrameBuffers();

	// Recreate the command buffers
	m_API.CreateCommandBuffers();
}

void VkRenderHelper::BeginFrame()
{
	VulkanWrapper::VkContainer& m_API = VulkanWrapper::VkContainer::instance();
	// Get the next image
	vk::Result const result = m_API.swapchainInfo.GetNextImage(m_API.deviceQueue.m_Device, m_API.syncObjectContainer.getImageAvailableSemaphore());

	// Check the result of getting the next image
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		// Recreate the swapchain and all the dependent resources
		recreateSwapChain();
	}
	else
	{
		VK_CHECK_RESULT(result);
	}

	// Set the current frame
	m_API.syncObjectContainer.setCurrentFrame(m_API.swapchainInfo.getCurrentFrameIndex());

	// We begin the command buffer
	const auto& commandBuffer = m_API.commandBuffers.at(m_API.swapchainInfo.getCurrentFrameIndex());
	commandBuffer.Begin();

	auto renderFunc = [this, &m_API, &commandBuffer]()
	{
		// Bind the graphics pipeline
		commandBuffer.BindPipeline(vk::PipelineBindPoint::eGraphics, m_API.graphicsPipeline);

		RenderMeshes();

		ImGuiVulkan::Render(m_API);
	};

	auto const& frameBuffer = m_API.renderPassFrameBuffers.m_Framebuffers.at(m_API.swapchainInfo.getCurrentFrameIndex());
	commandBuffer.DoRenderPass(m_API.renderPassFrameBuffers.m_RenderPass, frameBuffer, m_API.swapchainInfo.m_Extent, renderFunc);
}

void VkRenderHelper::EndFrame()
{
	VulkanWrapper::VkContainer& m_API = VulkanWrapper::VkContainer::instance();
	// Get the command buffer and submit info
	VulkanWrapper::CommandBuffer& commandBuffer = m_API.commandBuffers.at(m_API.syncObjectContainer.getCurrentFrame());

	vk::SubmitInfo const submitInfo = m_API.syncObjectContainer.getSubmitInfo(commandBuffer.get());

	// Reset the fence associated with the current image
	m_API.syncObjectContainer.resetFences();

	// Submit the command buffer
	m_API.deviceQueue.GetQueue(VulkanWrapper::QueueType::GRAPHICS).submit(submitInfo, m_API.syncObjectContainer.getInFlightFence());

	// Wait for the fence associated with the current image with a timeout of 1 second
	VK_CHECK_RESULT(m_API.syncObjectContainer.waitForFences())

	// Present the image
	vk::Result const result = m_API.deviceQueue.Present(m_API.swapchainInfo.m_Swapchain, m_API.syncObjectContainer.getCurrentFrame(), m_API.syncObjectContainer.getRenderFinishedSemaphore());
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		// Recreate the swapchain and all the dependent resources
		recreateSwapChain();
	}

	ClearMeshes();
}

void VkRenderHelper::AddMesh(const VulkanWrapper::Mesh& mesh, const glm::mat4& model)
{
	m_Meshes.emplace_back(mesh, model);
}

void VkRenderHelper::AddLight(LIGHT_TYPE type, void* light)
{
	m_Lights.push_back(std::tuple(type, light));
}

void VkRenderHelper::ClearLights()
{
	m_Lights.clear();
}

void VkRenderHelper::SetCamera(node::Camera* camera)
{
	m_Camera = camera;
}

void VkRenderHelper::ClearMeshes()
{
	m_Meshes.clear();
}

void VkRenderHelper::UpdateDescriptorSets(VulkanWrapper::Mesh& mesh, const glm::mat4& model, const std::vector<std::reference_wrapper<const PointLight>>& pointLights,
                                          const std::vector<std::reference_wrapper<const DirectionalLight>>& directionalLights, const std::vector<std::reference_wrapper<const SpotLight>>& spotLights)
{
	auto const& projection = m_Camera->getProjectionMatrix();
	auto const& view       = m_Camera->getViewMatrix();

	const VulkanWrapper::ModelViewProjection ubo = { projection * view * model };

	mesh.UpdateBuffers(ubo, pointLights, directionalLights, spotLights);
}

void VkRenderHelper::RenderMeshes()
{
	VulkanWrapper::VkContainer& m_API = VulkanWrapper::VkContainer::instance();

	// Create the 3 vectors for the lights
	std::vector<std::reference_wrapper<const PointLight>> pointLights;
	std::vector<std::reference_wrapper<const DirectionalLight>> directionalLights;
	std::vector<std::reference_wrapper<const SpotLight>> spotLights;

	using enum LIGHT_TYPE;
	
	// Fill the vectors with the lights (LIGHT_TYPE, void*)
	for (auto const& [type, light] : m_Lights)
	{
		switch (type)
		{
		case POINT: pointLights.emplace_back(*static_cast<PointLight*>(light)); break;
		case DIRECTIONAL: directionalLights.emplace_back(*static_cast<DirectionalLight*>(light)); break;
		case SPOT: spotLights.emplace_back(*static_cast<SpotLight*>(light)); break;
		}
	}

	// Fill all the remaining slots with default lights
	for (size_t i = pointLights.size(); i < MAX_POINT_LIGHTS; i++)
	{
		pointLights.emplace_back(m_DefaultPointLight);
	}

	for (size_t i = directionalLights.size(); i < MAX_DIRECTIONAL_LIGHTS; i++)
	{
		directionalLights.emplace_back(m_DefaultDirectionalLight);
	}

	for (size_t i = spotLights.size(); i < MAX_SPOT_LIGHTS; i++)
	{
		spotLights.emplace_back(m_DefaultSpotLight);
	}

	// This function will render all the meshes and update the descriptor sets for each mesh if needed
	for (auto& [mesh, modelMatrix] : m_Meshes)
	{
		// Update the descriptor sets
		UpdateDescriptorSets(mesh, modelMatrix, pointLights, directionalLights, spotLights);

		// Binds descriptor sets, vertex buffer and index buffer then Draw the mesh
		mesh.BindForDrawing(m_API.commandBuffers.at(m_API.swapchainInfo.getCurrentFrameIndex()).get(), m_API.pipelineLayout);
	}
}