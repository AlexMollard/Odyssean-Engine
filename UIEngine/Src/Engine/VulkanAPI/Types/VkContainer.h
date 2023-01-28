#pragma once

// This file contains all structs and classes that are used to store data in the VulkanAPI.

#include "../DescriptorManager.h"
#include "CommandBuffer.h"
#include "DeviceQueue.h"
#include "RenderPassFramebuffer.h"
#include "SwapchainInfo.h"
#include "SyncObjectContainer.h"
#include "Window.h"
#include "common.h"
#include <map>

namespace VulkanWrapper
{
struct Mesh;
}

namespace VulkanWrapper
{
struct VkContainer
{
	~VkContainer();

	// Vulkan API
	vk::InstanceCreateInfo instanceCreateInfo;
	vk::Instance           instance;

	// Extensions and Layers
	std::vector<const char*> extensions;
	std::vector<const char*> validationLayers;

	// Debug Messenger
	vk::DebugUtilsMessengerEXT debugMessenger;

	// Pipeline
	vk::PipelineLayout                      pipelineLayout;
	vk::Pipeline                            graphicsPipeline;
	std::map<std::string, vk::ShaderModule> shaderModules;

	std::map<std::string, VulkanWrapper::Texture*> textureCache;

	// Depth Buffer
	Texture depthTexture;

	bool enableValidationLayers = true;

	// UIEngine API
	DeviceQueue           deviceQueue;
	SwapchainInfo         swapchainInfo;
	RenderPassFramebuffer renderPassFrameBuffers;
	SyncObjectContainer   syncObjectContainer;

	vk::Device& device = deviceQueue.m_Device;

	vk::CommandPool            commandPool;
	std::vector<CommandBuffer> commandBuffers;

	// Descriptor set stuff
	VulkanWrapper::DescriptorManager* descriptorManager = nullptr;
	vk::DescriptorPool                imguiDescriptorPool;

	Window window;

	// Returns the swapchain
	vk::SwapchainKHR& Swapchain();

	CommandBuffer& CreateCommandBuffer();

	vk::ShaderModule CreateShaderModule(const char* shaderFile);

	void SetupViewportAndScissor(vk::Viewport& viewport, vk::Rect2D& scissor);

	vk::PipelineDepthStencilStateCreateInfo SetupDepthAndStencilState();

	void CreateSwapChain();
	void CreateRenderPass();
	void CreateDepthResources();
	void CreateFrameBuffers();
	void CreateCommandBuffers();
	void CreateGraphicsPipeline(const char* vertShader, const char* fragShader, Mesh& mesh);

	std::vector<char> ReadFile(const char* fileDir);
};
} // namespace VulkanWrapper