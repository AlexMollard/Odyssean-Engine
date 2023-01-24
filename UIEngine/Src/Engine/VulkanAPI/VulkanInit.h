#pragma once
#include "Types/VkContainer.h"
#include "vulkan/vulkan_enums.hpp"

class VulkanInit
{
public:
	VulkanInit() = default;
	~VulkanInit();

	void Initialize(const std::string& name, int width, int height);

	Window* GetWindow()
	{
		return &m_API.window;
	}
	vk::Device& GetDevice()
	{
		return m_API.deviceQueue.m_Device;
	}
	VulkanWrapper::VkContainer& GetAPI()
	{
		return m_API;
	}

private:
	vk::DebugUtilsMessengerCreateInfoEXT m_DebugMessengerCreateInfo;
	std::vector<vk::ExtensionProperties> m_SupportedExtensions;
	std::vector<vk::LayerProperties>     m_SupportedLayers;

	void CreateExtensions();
	void GetRequiredExtensions(vk::InstanceCreateInfo& createInfo);
	void CheckRequiredExtensions(const std::vector<const char*>& requiredExtensions, const std::vector<vk::ExtensionProperties>& supportedExtensions);
	bool IsExtensionSupported(const char* extension, const std::vector<vk::ExtensionProperties>& supportedExtensions);
	void CheckRequiredLayers(const std::vector<const char*>& requiredLayers, const std::vector<vk::LayerProperties>& supportedLayers);
	bool IsLayerSupported(const char* layer, const std::vector<vk::LayerProperties>& supportedLayers);

	// Init functions for the API
	void InitInstance();
	void InitDevice();
	void InitSurface();
	
	VulkanWrapper::VkContainer m_API;
};
