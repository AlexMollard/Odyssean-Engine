#include "pch.h"

#include "VulkanInit.h"

#include <algorithm>
#include <iostream>
#include <ranges>

VulkanInit::~VulkanInit()
{
	m_API.descriptorManager->cleanup();
	m_API.descriptorManager = nullptr;
}

void VulkanInit::Initialize(const std::string& name, int width, int height)
{
	// Setup the Vulkan window
	m_API.window.Initialize(name.c_str(), width, height);

	// Init the API
	InitInstance();
	InitDevice();
}

void VulkanInit::OutputSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
{
	switch (severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		std::cout << "\033[1;34m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << "\033[0m ";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		std::cout << "\033[1;33m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << "\033[0m ";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		std::cout << "\033[1;31m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << "\033[0m ";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		std::cout << "\033[1;32m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << "\033[0m ";
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		std::cout << "\033[1;37m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << "\033[0m ";
		break;
	default: std::cout << "\033[1;37m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << "\033[0m "; break;
	}
}

void VulkanInit::OutputMessageType(VkDebugUtilsMessageTypeFlagsEXT type)
{
	switch (type)
	{
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		std::cout << "\033[1;32m" << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(type)) << "\033[0m" << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		std::cout << "\033[1;33m" << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(type)) << "\033[0m" << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		std::cout << "\033[1;31m" << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(type)) << "\033[0m" << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT:
		std::cout << "\033[1;37m" << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(type)) << "\033[0m" << std::endl;
		break;
	default: std::cout << "\033[1;37m" << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(type)) << "\033[0m" << std::endl; break;
	}
}

void VulkanInit::CreateExtensions()
{
	// Add all extensions and layers to the API
	m_API.extensions       = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
	m_API.validationLayers = { "VK_LAYER_KHRONOS_validation" };

	// Create the debug messenger create info
	m_DebugMessengerCreateInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	m_DebugMessengerCreateInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
	                                          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
	m_DebugMessengerCreateInfo.setPUserData(nullptr);

	// Set the callback function
	PFN_vkDebugUtilsMessengerCallbackEXT callback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
	                                                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
	{
		// Print message name
		std::cout << "Validation layer: " << pCallbackData->pMessageIdName << std::endl;

		// Print message object type
		std::cout << "Object type: \033[4;37m" << vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects->objectType)) << "\033[0m" << std::endl;

		// Print message
		std::cout << "Message: " << pCallbackData->pMessage << std::endl;

		// Print message description
		if (pCallbackData->cmdBufLabelCount > 0)
		{
			std::cout << "Description: " << pCallbackData->pCmdBufLabels->pLabelName << std::endl;
		}

		// Print message severity with color
		std::cout << "Severity: ";
		VulkanInit::OutputSeverity(messageSeverity);

		// Print message type with color
		std::cout << "| Type: ";
		VulkanInit::OutputMessageType(messageType);

		std::cout << std::endl;
		return VK_FALSE;
	};

	m_DebugMessengerCreateInfo.setPfnUserCallback(callback);
}

void VulkanInit::GetRequiredExtensions(vk::InstanceCreateInfo& createInfo)
{
	CreateExtensions();

	// Get all supported extensions
	m_SupportedExtensions = vk::enumerateInstanceExtensionProperties();

	// Get all supported layers
	m_SupportedLayers = vk::enumerateInstanceLayerProperties();

	// Add the extensions to the API (must not go out of range)
	for (const auto& extension : Window::GetRequiredExtensions())
	{
		m_API.extensions.push_back(extension);
	}

	// Add the extensions to the create info
	createInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_API.extensions.size()));
	createInfo.setPpEnabledExtensionNames(m_API.extensions.data());

	// Check if the required extensions are supported
	CheckRequiredExtensions(m_API.extensions, m_SupportedExtensions);

	// Check if the required validation layers are supported
	CheckRequiredLayers(m_API.validationLayers, m_SupportedLayers);

	// Add the validation layers to the create info
	createInfo.setEnabledLayerCount(static_cast<uint32_t>(m_API.validationLayers.size()));
	createInfo.setPpEnabledLayerNames(m_API.validationLayers.data());
}

void VulkanInit::CheckRequiredLayers(const std::vector<const char*>& requiredLayers, const std::vector<vk::LayerProperties>& supportedLayers) const
{
	for (const auto& requiredLayer : requiredLayers)
	{
		S_ASSERT(IsLayerSupported(requiredLayer, supportedLayers), "Required layer is not supported: " + std::string(requiredLayer))
	}
}

bool VulkanInit::IsLayerSupported(const char* layer, const std::vector<vk::LayerProperties>& supportedLayers) const
{
	bool isSupported = true;

	for (const auto& supportedLayer : supportedLayers)
	{
		if (strcmp(layer, supportedLayer.layerName) == 0)
		{
			isSupported = true;
			break;
		}
		isSupported = false;
	}

	return isSupported;
}

// Checks if the required extensions are supported
void VulkanInit::CheckRequiredExtensions(const std::vector<const char*>& requiredExtensions, const std::vector<vk::ExtensionProperties>& supportedExtensions) const
{
	for (const auto& requiredExtension : requiredExtensions)
	{
		S_ASSERT(IsExtensionSupported(requiredExtension, supportedExtensions), "Required extension is not supported: " + std::string(requiredExtension))
	}
}

bool VulkanInit::IsExtensionSupported(const char* extension, const std::vector<vk::ExtensionProperties>& supportedExtensions) const
{
	bool isSupported = true;

	for (const auto& supportedExtension : supportedExtensions)
	{
		if (strcmp(extension, supportedExtension.extensionName) == 0)
		{
			isSupported = true;
			break;
		}
		isSupported = false;
	}

	return isSupported;
}

void VulkanInit::InitInstance()
{
	m_API.instanceCreateInfo = vk::InstanceCreateInfo();

	// Get the required extensions for the API
	GetRequiredExtensions(m_API.instanceCreateInfo);

	// Add application info to the instance create info
	vk::ApplicationInfo appInfo;
	appInfo.setPApplicationName("UIEngine");
	appInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
	appInfo.setPEngineName("UIEngine");
	appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
	appInfo.setApiVersion(VK_API_VERSION_1_0);
	m_API.instanceCreateInfo.setPApplicationInfo(&appInfo);

	// We add the following line to all the create info structs that we pass to Vulkan
	// So that we can debug our application
	m_API.instanceCreateInfo.setPNext(&m_DebugMessengerCreateInfo);

	// Create the Vulkan instance
	m_API.vulkanInstance = vk::createInstance(m_API.instanceCreateInfo);
}

void VulkanInit::InitDevice()
{
	// Get the physical device
	m_API.deviceQueue.m_PhysicalDevice = m_API.vulkanInstance.enumeratePhysicalDevices().front();
	InitSurface();

	VulkanWrapper::DeviceQueue& deviceQueue = m_API.deviceQueue;

	// Now set all the queues on the deviceQueue, to do this we need to loop through all the queue families
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_API.deviceQueue.m_PhysicalDevice.getQueueFamilyProperties();

	// Loop through all the queue families
	for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
	{
		// Get the queue family properties
		vk::QueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];

		// Check if the queue family supports graphics
		if ((queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics) && !deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::GRAPHICS))
		{
			deviceQueue.SetQueueFamilyIndex(VulkanWrapper::QueueType::GRAPHICS, i);
			continue;
		}

		// Check if the queue family supports presentation
		if (m_API.deviceQueue.m_PhysicalDevice.getSurfaceSupportKHR(i, m_API.window.m_Surface) && !deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::PRESENT))
		{
			deviceQueue.SetQueueFamilyIndex(VulkanWrapper::QueueType::PRESENT, i);
			continue;
		}

		// Check if the queue family supports compute
		if ((queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute) && !deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::COMPUTE))
		{
			deviceQueue.SetQueueFamilyIndex(VulkanWrapper::QueueType::COMPUTE, i);
			continue;
		}

		// Check if the queue family supports transfer
		if ((queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer) && !deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::TRANSFER))
		{
			deviceQueue.SetQueueFamilyIndex(VulkanWrapper::QueueType::TRANSFER, i);
			continue;
		}
	}

	// Check if all the queues have been set
	S_ASSERT(deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::GRAPHICS), "No graphics queue family index found!")
	S_ASSERT(deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::PRESENT), "No present queue family index found!")

	// Get the queue priorities
	std::vector<float> queuePriorities = { 1.0f };

	// Create the queue create infos
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::GRAPHICS),
	                              static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data());
	queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::PRESENT),
	                              static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data());

	if (deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::TRANSFER))
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::TRANSFER),
		                              static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data());

	if (deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::COMPUTE))
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::COMPUTE),
		                              static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data());

	// Create the device create info
	auto deviceCreateInfo = vk::DeviceCreateInfo(vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data());

	// Set the device features
	auto deviceFeatures = vk::PhysicalDeviceFeatures();
	deviceCreateInfo.setPEnabledFeatures(&deviceFeatures);

	// Set the device extensions
	if (m_API.deviceQueue.m_DeviceExtensions.empty())
	{
		// Add the minimum required extensions
		m_API.deviceQueue.m_DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	std::vector<vk::ExtensionProperties> availableDeviceExtensions = m_API.deviceQueue.m_PhysicalDevice.enumerateDeviceExtensionProperties();

	// Check if the device extensions are supported
	CheckRequiredExtensions(m_API.deviceQueue.m_DeviceExtensions, availableDeviceExtensions);

	deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_API.deviceQueue.m_DeviceExtensions.size()));
	deviceCreateInfo.setPpEnabledExtensionNames(m_API.deviceQueue.m_DeviceExtensions.data());

	// Create the device
	m_API.deviceQueue.m_Device = m_API.deviceQueue.m_PhysicalDevice.createDevice(deviceCreateInfo);

	// Get the queues
	deviceQueue.SetQueue(VulkanWrapper::QueueType::GRAPHICS, m_API.deviceQueue.m_Device.getQueue(deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::GRAPHICS), 0));
	deviceQueue.SetQueue(VulkanWrapper::QueueType::PRESENT, m_API.deviceQueue.m_Device.getQueue(deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::PRESENT), 0));

	if (deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::TRANSFER))
		deviceQueue.SetQueue(VulkanWrapper::QueueType::TRANSFER, m_API.deviceQueue.m_Device.getQueue(deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::TRANSFER), 0));

	if (deviceQueue.IsQueueFamilyIndexSet(VulkanWrapper::QueueType::COMPUTE))
		deviceQueue.SetQueue(VulkanWrapper::QueueType::COMPUTE, m_API.deviceQueue.m_Device.getQueue(deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::COMPUTE), 0));

	// Set the device queue
	m_API.deviceQueue = deviceQueue;

	// Display to the user there graphics cards optimal variables
	std::cout << "Optimal tiling alignment: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().limits.optimalBufferCopyOffsetAlignment << std::endl;
	std::cout << "Optimal tiling granularity: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().limits.optimalBufferCopyRowPitchAlignment << std::endl;
	// Best uniform buffer alignment
	std::cout << "Best uniform buffer alignment: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().limits.minUniformBufferOffsetAlignment << std::endl;
	// Best storage buffer alignment
	std::cout << "Best storage buffer alignment: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().limits.minStorageBufferOffsetAlignment << std::endl;
}

void VulkanInit::InitSurface()
{
	// Create the surface using glfw
	VkSurfaceKHR tmpSurface = {};
	S_ASSERT(glfwCreateWindowSurface(m_API.vulkanInstance, m_API.window.GetWindow(), nullptr, &tmpSurface) == VK_SUCCESS, "failed ot create window surface!")

	Window& window = m_API.window;

	window.m_Surface = tmpSurface;

	// Get the surface capabilities
	window.m_SurfaceCapabilities = m_API.deviceQueue.m_PhysicalDevice.getSurfaceCapabilitiesKHR(window.m_Surface);

	// Get the surface formats
	window.m_SurfaceFormats = m_API.deviceQueue.m_PhysicalDevice.getSurfaceFormatsKHR(window.m_Surface);

	// Get the surface present modes
	window.m_SurfacePresentModes = m_API.deviceQueue.m_PhysicalDevice.getSurfacePresentModesKHR(window.m_Surface);

	// Get the surface extent
	window.m_SurfaceExtent = window.m_SurfaceCapabilities.currentExtent;

	// Get the surface format
	window.m_SurfaceFormat = window.m_SurfaceFormats.front();

	// Get the surface present mode
	window.m_SurfacePresentMode = vk::PresentModeKHR::eFifo;

	// Get the surface image count
	window.m_SurfaceImageCount = window.m_SurfaceCapabilities.minImageCount + 1;
	if (window.m_SurfaceCapabilities.maxImageCount > 0 && window.m_SurfaceImageCount > window.m_SurfaceCapabilities.maxImageCount)
	{
		window.m_SurfaceImageCount = window.m_SurfaceCapabilities.maxImageCount;
	}
}