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
	~API();

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

	// Returns the swapchain
	vk::SwapchainKHR& Swapchain();

	// Returns the current command buffer
	const vk::CommandBuffer& GetCurrentCommandBuffer();

	CommandBuffer& CreateCommandBuffer();

	void CreateSwapChain();
	void CreateRenderPass();
	void CreateFrameBuffers();
	void CreateCommandBuffers();
	void CreateSyncObjects();
};
} // namespace vulkan