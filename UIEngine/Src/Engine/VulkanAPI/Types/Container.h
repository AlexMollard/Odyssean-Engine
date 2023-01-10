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

	vk::PhysicalDevice physicalDevice;
	vk::Device         device;
	std::vector<const char*> deviceExtensions;

	// Extensions and Layers
	std::vector<const char*> extensions;
	std::vector<const char*> validationLayers;

	// Debug Messenger
	vk::DebugUtilsMessengerEXT debugMessenger;

	// Surface
	vk::SurfaceKHR                    surface;
	vk::SurfaceCapabilitiesKHR        surfaceCapabilities;
	vk::Extent2D                      surfaceExtent;
	vk::SurfaceFormatKHR              surfaceFormat;
	vk::PresentModeKHR                surfacePresentMode;
	size_t                            surfaceImageCount;
	std::vector<vk::SurfaceFormatKHR> surfaceFormats;
	std::vector<vk::PresentModeKHR>   surfacePresentModes;

	// Swapchain
	vk::SwapchainKHR               swapchain;
	vk::Format                     swapchainFormat;
	vk::Extent2D                   swapchainExtent;
	
	vk::RenderPass                 renderPass;
	vk::PipelineLayout             pipelineLayout;
	vk::Pipeline                   graphicsPipeline;
	vk::CommandPool                commandPool;
	std::vector<vk::Image>         swapchainImages;
	std::vector<vk::ImageView>     swapchainImageViews;
	std::vector<vk::Framebuffer>   swapchainFramebuffers;
	std::vector<vk::CommandBuffer> commandBuffers;
	size_t                         currentFrame           = 0;
	bool                           framebufferResized     = false;
	bool                           enableValidationLayers = true;

	// UIEngine API
	DeviceQueue           deviceQueue;
	SwapchainInfo         swapchainInfo;
	RenderPassFramebuffer renderPassFrameBuffer;
	CommandBuffer         commandBuffer;
	SemaphoreFence        semaphoreFence;

	Window window;

	~API()
	{
		device.waitIdle();
		// Image views
		for (auto imageView : swapchainImageViews)
		{
			device.destroy(imageView);
		}
		// Swapchain
		device.destroy(swapchain);
		// Surface
		instance.destroy(surface);
		device.destroy();
		instance.destroy();
		window.Destroy();
	}
};
} // namespace vulkan