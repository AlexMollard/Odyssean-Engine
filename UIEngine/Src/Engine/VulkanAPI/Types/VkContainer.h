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
#include <Engine/Renderer.h>

namespace VulkanWrapper
{
struct Mesh;
}

namespace VulkanWrapper
{
class VkContainer
{
	VkContainer(){};
	VkContainer(const VkContainer&)            = delete;
	VkContainer& operator=(const VkContainer&) = delete;

public:
	~VkContainer();

	inline static VkContainer& instance()
	{
		static VkContainer instance;
		return instance;
	}

	// Vulkan API
	vk::InstanceCreateInfo instanceCreateInfo;
	vk::Instance           vulkanInstance;

	// Extensions and Layers
	std::vector<const char*> extensions;
	std::vector<const char*> validationLayers;

	// Debug Messenger
	vk::DebugUtilsMessengerEXT debugMessenger;

	// Pipeline
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline       graphicsPipeline;

	std::map<std::string, vk::ShaderModule, std::less<>>        shaderModules;
	std::map<std::string, VulkanWrapper::Texture*, std::less<>> textureCache;

	// Depth Buffer
	Texture depthTexture;

	bool enableValidationLayers = true;

	// UIEngine API
	DeviceQueue           deviceQueue;
	SwapchainInfo         swapchainInfo;
	RenderPassFramebuffer renderPassFrameBuffers;
	SyncObjectContainer   syncObjectContainer;
	Renderer::VulkanImpl* renderer;

	vk::Device& device = deviceQueue.m_Device;

	vk::CommandPool            commandPool;
	std::vector<CommandBuffer> commandBuffers;
	std::vector<CommandBuffer> commandBuffersNoDepth;

	// Descriptor set stuff
	std::shared_ptr<VulkanWrapper::DescriptorManager> descriptorManager = nullptr;
	vk::DescriptorPool                                imguiDescriptorPool;

	Window window;

	// Returns the swapchain
	vk::SwapchainKHR& Swapchain();

	CommandBuffer& CreateCommandBuffer();

	vk::ShaderModule CreateShaderModule(const char* shaderFile);

	void SetupViewportAndScissor(vk::Viewport& viewport, vk::Rect2D& scissor);

	vk::PipelineDepthStencilStateCreateInfo SetupDepthAndStencilState();

	void SetDescriptorManager(std::shared_ptr<VulkanWrapper::DescriptorManager> newDescriptorManager);
	void CreateSwapChain();
	void CreateRenderPasses();
	void CreateDepthResources();
	void CreateFrameBuffers();
	void CreateCommandBuffers();
	void CreateGraphicsPipeline(const char* vertShader, const char* fragShader, std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> descriptorSetLayouts);
	void SetVulkanRenderer(Renderer::VulkanImpl* newVulkanRenderer);

	std::vector<char> ReadFile(const char* fileDir);
};
} // namespace VulkanWrapper