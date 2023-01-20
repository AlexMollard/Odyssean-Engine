#pragma once

// This file contains all structs and classes that are used to store data in the VulkanAPI.

#include "CommandBuffer.h"
#include "DeviceQueue.h"
#include "RenderPassFramebuffer.h"
#include "SwapchainInfo.h"
#include "SyncObjectContainer.h"
#include "Window.h"
#include <map>
#include "common.h"

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

	// Pipeline
	vk::PipelineLayout                      graphicsPipelineLayout;
	vk::Pipeline                            graphicsPipeline;
	std::map<std::string, vk::ShaderModule> shaderModules;

	// Depth Buffer
	Texture depthTexture;

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

	vk::ShaderModule CreateShaderModule(const char* shaderFile);

	void SetupViewportAndScissor(vk::Viewport& viewport, vk::Rect2D& scissor);
	void SetupPipelineLayout(vk::PipelineLayout& pipelineLayout, vk::DescriptorSetLayout& descriptorSetLayout);
	vk::PipelineDepthStencilStateCreateInfo SetupDepthAndStencilState();
	
	void CreateSwapChain();
	void CreateRenderPass();
	void CreateDepthResources();
	void CreateFrameBuffers();
	void CreateCommandBuffers();
	void CreateGraphicsPipeline(const char* vertShader, const char* fragShader, vk::DescriptorSetLayout& descriptorSetLayout);

	std::vector<char> ReadFile(const char* fileDir);
};
} // namespace vulkan