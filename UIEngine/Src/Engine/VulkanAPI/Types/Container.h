#pragma once

// This file contains all structs and classes that are used to store data in the VulkanAPI.

#include "CommandBuffer.h"
#include "DeviceQueue.h"
#include "RenderPassFramebuffer.h"
#include "SemaphoreFence.h"
#include "SwapchainInfo.h"
#include "Window.h"

namespace vulkan
{
struct API
{
	// Vulkan API
	vk::InstanceCreateInfo instanceCreateInfo;
	vk::Instance           instance;

	// Extensions and Layers
	std::vector<const char*> extensions;
	std::vector<const char*> validationLayers;

	// Debug Messenger
	vk::DebugUtilsMessengerEXT debugMessenger;

	vk::PipelineLayout pipelineLayout;
	vk::Pipeline       graphicsPipeline;
	vk::CommandPool    commandPool;
	size_t             currentFrame           = 0;
	bool               framebufferResized     = false;
	bool               enableValidationLayers = true;

	// UIEngine API
	DeviceQueue                deviceQueue;
	SwapchainInfo              swapchainInfo;
	RenderPassFramebuffer      renderPassFrameBuffers;
	std::vector<CommandBuffer> commandBuffers;
	SemaphoreFence             semaphoreFence;

	Window window;

	~API()
	{
		// Wait for everything to be free
		deviceQueue.wait();

		// Semaphores
		deviceQueue.m_Device.destroy(semaphoreFence.m_ImageAvailable);
		deviceQueue.m_Device.destroy(semaphoreFence.m_RenderFinished);

		// Fence
		deviceQueue.m_Device.destroy(semaphoreFence.m_InFlight);
		for (auto fence : semaphoreFence.m_ImagesInFlight) { deviceQueue.m_Device.destroy(fence); }

		// Command buffers
		for (auto commandBuffer : commandBuffers) { deviceQueue.m_Device.freeCommandBuffers(commandPool, commandBuffer.get()); }
		// Command pool
		deviceQueue.m_Device.destroy(commandPool);
		// renderPass
		deviceQueue.m_Device.destroy(renderPassFrameBuffers.m_RenderPass);
		// Framebuffers
		for (auto framebuffer : renderPassFrameBuffers.m_Framebuffers) { deviceQueue.m_Device.destroy(framebuffer); }
		// Image views
		for (auto imageView : swapchainInfo.m_ImageViews) { deviceQueue.m_Device.destroy(imageView); }
		// Swapchain
		deviceQueue.m_Device.destroy(swapchainInfo.m_Swapchain);
		// Surface
		instance.destroy(window.m_Surface);
		deviceQueue.m_Device.destroy();
		instance.destroy();
		window.Destroy();
	}
};
} // namespace vulkan