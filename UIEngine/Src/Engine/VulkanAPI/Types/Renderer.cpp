#include "Renderer.h"

#include <iostream>

vulkan::Renderer::~Renderer()
{
	Destroy();
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
}

void vulkan::Renderer::Destroy()
{}

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

	m_API->commandBuffers[m_API->swapchainInfo.getCurrentFrameIndex()].DoRenderPass(m_API->renderPassFrameBuffers.m_RenderPass, m_API->renderPassFrameBuffers.m_Framebuffers[m_API->swapchainInfo.getCurrentFrameIndex()],
																					m_API->swapchainInfo.m_Extent, []() {
		// Perform some render pass operations here, such as binding pipeline, binding vertex buffers etc.
	});

	// We end the command buffer
	m_API->commandBuffers[m_API->swapchainInfo.getCurrentFrameIndex()].End();

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
