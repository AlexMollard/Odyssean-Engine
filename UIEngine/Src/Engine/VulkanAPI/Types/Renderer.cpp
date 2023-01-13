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

	// Create the command buffers
	m_API->CreateCommandBuffers();

	// Create the sync objects
	m_API->CreateSyncObjects();
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
	// Get the current image index
	uint32_t imageIndex = m_API->currentFrame;

	// Get the next image
	vk::Result result = m_API->swapchainInfo.GetNextImage(m_API->deviceQueue.m_Device, m_API->semaphoreFence.m_ImageAvailable);

	// If the result is not success we throw an error
	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) { throw std::runtime_error("Failed to acquire next image"); }

	// We get the command buffer
	vulkan::CommandBuffer& commandBuffer = m_API->commandBuffers[imageIndex];

	// We begin the command buffer
	commandBuffer.Begin();

	// We begin the render pass
	commandBuffer.BeginRenderPass(m_API->renderPassFrameBuffers.m_RenderPass, m_API->swapchainInfo.m_Extent, m_API->renderPassFrameBuffers.m_Framebuffers[imageIndex]);

	// We set the viewport
	commandBuffer.SetViewport(m_API->swapchainInfo.m_Extent);

	// We set the scissor
	commandBuffer.SetScissor(m_API->swapchainInfo.m_Extent);

	// This is where you would record the commands to the command buffer
	// We end the render pass
	commandBuffer.EndRenderPass();

	// We end the command buffer
	commandBuffer.End();
}


void vulkan::Renderer::EndFrame()
{
	vk::Device& device = m_API->deviceQueue.m_Device;

	// Get the current image index
	uint32_t imageIndex = m_API->currentFrame;

	// Get the command buffer and submit info
	vulkan::CommandBuffer& commandBuffer = m_API->commandBuffers[imageIndex];
	vk::SubmitInfo         submitInfo    = m_API->semaphoreFence.GetSubmitInfo(commandBuffer.get(), imageIndex);

	// Reset the fence associated with the current image
	m_API->semaphoreFence.ResetFence(device, m_API->semaphoreFence.m_ImagesInFlight);

	// Submit the command buffer and wait for it to be finished
	m_API->deviceQueue.GetQueue(QueueType::GRAPHICS).submit(submitInfo, m_API->semaphoreFence.m_ImagesInFlight[imageIndex]);

	// Wait for the graphics queue to be idle
	m_API->deviceQueue.GetQueue(QueueType::GRAPHICS).waitIdle();

	// Present the image
	vk::Result result = m_API->deviceQueue.Present(m_API->swapchainInfo.m_Swapchain, imageIndex, m_API->semaphoreFence.m_RenderFinished);

	// If the result is not success we throw an error
	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) { throw std::runtime_error("Failed to present image"); }

	// Increment the current frame
	m_API->currentFrame = (m_API->currentFrame + 1) % m_API->swapchainInfo.m_Images.size();
}

void vulkan::Renderer::RenderUI()
{}
