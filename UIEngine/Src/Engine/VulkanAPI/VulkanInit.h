#pragma once
#include "Types/Container.h"

// This file is used to initialize the Vulkan API and store it in the API struct.
class VulkanInit
{
public:
	VulkanInit() = default;
	~VulkanInit();

	void Initialize(std::string name, int width, int height);

	Window* GetWindow()
	{
		return &m_API.window;
	};

	vk::Device& GetDevice()
	{
		return m_API.deviceQueue.m_Device;
	}

	vulkan::API m_API;

private:

	vk::DebugUtilsMessengerCreateInfoEXT m_DebugMessengerCreateInfo;

	// All supported extensions
	std::vector<vk::ExtensionProperties> m_SupportedExtensions;

	// All supported layers
	std::vector<vk::LayerProperties> m_SupportedLayers;

	// Gets the required extensions for the API
	void CreateExtensions();
	void GetRequiredExtensions(vk::InstanceCreateInfo& createInfo);

	// extension check
	void CheckRequiredExtensions(const std::vector<const char*>& requiredExtensions, const std::vector<vk::ExtensionProperties>& supportedExtensions);
	bool IsExtensionSupported(const char* extension, const std::vector<vk::ExtensionProperties>& supportedExtensions);

	// layer check
	void CheckRequiredLayers(const std::vector<const char*>& requiredLayers, const std::vector<vk::LayerProperties>& supportedLayers);
	bool IsLayerSupported(const char* layer, const std::vector<vk::LayerProperties>& supportedLayers);

	// Init functions for the API
	void InitInstance();
	void InitDevice();
	void InitSurface();
};
