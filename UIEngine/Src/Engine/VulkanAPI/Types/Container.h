#pragma once

// This file contains all structs and classes that are used to store data in the VulkanAPI.

#include "CommandBuffer.h"
#include "DeviceQueue.h"
#include "RenderPassFramebuffer.h"
#include "SyncObjectContainer.h"
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

	bool enableValidationLayers = true;

	// UIEngine API
	DeviceQueue           deviceQueue;
	SwapchainInfo         swapchainInfo;
	RenderPassFramebuffer renderPassFrameBuffers;
	SyncObjectContainer   syncObjectContainer;

	vk::CommandPool            commandPool;
	std::vector<CommandBuffer> commandBuffers;

	Window window;

	// Returns the swapchain
	vk::SwapchainKHR& Swapchain();

	CommandBuffer& CreateCommandBuffer();

	void CreateSwapChain();
	void CreateRenderPass();
	void CreateFrameBuffers();
	void CreateCommandBuffers();
};
} // namespace vulkan