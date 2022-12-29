#pragma once

#include "VulkanRenderer.h"
#include <optional>
#include <vulkan/vulkan.hpp>

#include "Engine/BaseEngine.h"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

struct GLFWwindow;
class VulkanEngine : BaseEngine
{
public:
	VulkanEngine() = default;
	~VulkanEngine();

	void Init(const char* windowName, int width, int height);

	vk::CommandBuffer BeginSingleTimeCommands();
	// Returns DT
	float Update();
	void Render();

	void CreateMeshBuffer();

	bool GetClose() const { return m_close; }

	void* GetWindow() const { return m_Window; }

	void DrawFrame();

	const VulkanRenderer* GetRenderer() const { return &m_Renderer; }

private:

	GLFWwindow* m_Window;
	bool m_close = false;

	VulkanRenderer m_Renderer;

	// vertex Buffer and index buffer
	vk::Buffer m_VertexBuffer;
	uint32_t m_VertexBufferSize;
	vk::DeviceMemory m_VertexBufferMemory;
	vk::MemoryRequirements m_VertexBufferMemoryRequirements;

	vk::Buffer m_IndexBuffer;
	uint32_t m_IndexBufferSize;
	vk::DeviceMemory m_IndexBufferMemory;
	vk::MemoryRequirements m_IndexBufferMemoryRequirements;


	// Vulkan
	vk::Instance m_Instance;
	vk::DebugUtilsMessengerEXT m_DebugMessenger;
	vk::PhysicalDevice m_PhysicalDevice = vk::PhysicalDevice();
	vk::Device m_Device;
	vk::Queue m_GraphicsQueue;
	vk::Queue m_PresentQueue;
	vk::SurfaceKHR m_Surface;
	vk::SwapchainKHR m_SwapChain;
	std::vector<vk::Image> m_SwapChainImages;
	vk::Format m_SwapChainImageFormat;
	vk::Extent2D m_SwapChainExtent;
	std::vector<vk::ImageView> m_SwapChainImageViews;
	vk::Image m_DepthImage;
	vk::ImageView m_DepthImageView;
	vk::RenderPass m_RenderPass;
	vk::PipelineLayout m_PipelineLayout;
	vk::Pipeline m_GraphicsPipeline;
	std::vector<vk::Framebuffer> m_SwapChainFramebuffers;
	vk::CommandPool m_CommandPool;
	std::vector<vk::CommandBuffer> m_CommandBuffers;
	std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
	std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
	vk::DescriptorSetLayout m_DescriptorSetLayout;
	std::vector<vk::Fence> m_InFlightFences;
	std::vector<vk::Fence> m_ImagesInFlight;
	vk::DeviceMemory m_DepthImageMemory;
	vk::DescriptorPool m_DescriptorPool;
	std::vector<vk::DescriptorSet> m_DescriptorSets;


	size_t m_CurrentFrame = 0;
	bool m_FramebufferResized = false;
		
	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// Vulkan functions
	void CreateInstance();
	void SetupDebugMessenger();
	void CreateSurface();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateSyncObjects();
	void CreateDescriptorSetLayout();
	void CreateDepthResources();
	vk::Buffer CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);


	vk::MemoryAllocateInfo FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	vk::ShaderModule CreateShaderModule(std::vector<char> vertShaderCode);
	vk::ImageView CreateImageView(vk::Image m_SwapChainImage, vk::Format m_SwapChainImageFormat, vk::ImageAspectFlagBits param3, int param4);
	vk::Format FindDepthFormat();
	vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
	void CleanupSwapChain();
	void Cleanup();
	void RecreateSwapChain();
	void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
	void BindVertexBuffer(vk::CommandBuffer commandBuffer);
	void RecordCommandsToDrawQuad(vk::CommandBuffer commandBuffer);

	bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
	bool IsDeviceSuitable(vk::PhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device);
	vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	static std::vector<char> ReadFile(const std::string& filename);

	// Vulkan debug
	const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& debugCreateInfo);
	vk::Result CreateDebugUtilsMessengerEXT(const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator);
#ifdef NDEBUG
	const bool m_EnableValidationLayers = false;
#else
	const bool m_EnableValidationLayers = true;
#endif
};
