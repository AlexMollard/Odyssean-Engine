#include "VulkanInit.h"

#include <iostream>

VulkanInit::~VulkanInit()
{}

void VulkanInit::Initialize(std::string name, int width, int height)
{
	// Setup the Vulkan window
	m_API.window.Initialize(name.c_str(), width, height);

	// Init the API
	InitInstance();
	InitDevice();
}

void VulkanInit::CreateExtensions()
{
	// Add all extensions and layers to the API
	m_API.extensions       = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
	m_API.validationLayers = { "VK_LAYER_KHRONOS_validation" };

	// Create the debug messenger create info
	m_DebugMessengerCreateInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	m_DebugMessengerCreateInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
	m_DebugMessengerCreateInfo.setPUserData(nullptr);

	// Set the callback function
	PFN_vkDebugUtilsMessengerCallbackEXT callback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
													   void* pUserData) -> VkBool32 {
		// Print message name
		std::cout << "Validation layer: " << pCallbackData->pMessageIdName << std::endl;

		// Print message object type
		std::cout << "Object type: \033[4;37m" << vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects->objectType)) << "\033[0m" << std::endl;

		// Print message
		std::cout << "Message: " << pCallbackData->pMessage << std::endl;

		// Print message description
		if (pCallbackData->cmdBufLabelCount > 0) { std::cout << "Description: " << pCallbackData->pCmdBufLabels->pLabelName << std::endl; }

		// Print message severity with color
		std::cout << "Severity: ";
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: std::cout << "\033[1;34m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << "\033[0m "; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: std::cout << "\033[1;33m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << "\033[0m "; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: std::cout << "\033[1;31m" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << "\033[0m "; break;
		}

		// Print message type with color
		std::cout << "| Type: ";
		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: std::cout << "\033[1;32m" << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageType)) << "\033[0m" << std::endl; break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: std::cout << "\033[1;33m" << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageType)) << "\033[0m" << std::endl; break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: std::cout << "\033[1;31m" << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageType)) << "\033[0m" << std::endl; break;
		}

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

	// Get the required extensions
	auto glfwExtensions = m_API.window.GetRequiredExtensions();

	// Add the extensions to the API (must not go out of range)
	for (const auto& extension : glfwExtensions) { m_API.extensions.push_back(extension); }

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

void VulkanInit::CheckRequiredLayers(const std::vector<const char*>& requiredLayers, const std::vector<vk::LayerProperties>& supportedLayers)
{
	for (const auto& requiredLayer : requiredLayers)
	{
		if (!IsLayerSupported(requiredLayer, supportedLayers)) { throw std::runtime_error("Required layer is not supported: " + std::string(requiredLayer)); }
	}
}

bool VulkanInit::IsLayerSupported(const char* layer, const std::vector<vk::LayerProperties>& supportedLayers)
{
	for (const auto& supportedLayer : supportedLayers)
	{
		if (strcmp(layer, supportedLayer.layerName) == 0) { return true; }
	}

	return false;
}

// Checks if the required extensions are supported
void VulkanInit::CheckRequiredExtensions(const std::vector<const char*>& requiredExtensions, const std::vector<vk::ExtensionProperties>& supportedExtensions)
{
	for (const auto& requiredExtension : requiredExtensions)
	{
		if (!IsExtensionSupported(requiredExtension, supportedExtensions)) { throw std::runtime_error("Required extension is not supported: " + std::string(requiredExtension)); }
	}
}

bool VulkanInit::IsExtensionSupported(const char* extension, const std::vector<vk::ExtensionProperties>& supportedExtensions)
{
	for (const auto& supportedExtension : supportedExtensions)
	{
		if (strcmp(extension, supportedExtension.extensionName) == 0) { return true; }
	}

	return false;
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
	m_API.instance = vk::createInstance(m_API.instanceCreateInfo);

	// Output application info
	std::cout << "Vulkan application info:" << std::endl;
	std::cout << "\tApplication name: " << appInfo.pApplicationName << std::endl;
	std::cout << "\tApplication version: " << appInfo.applicationVersion << std::endl;
	std::cout << "\tEngine name: " << appInfo.pEngineName << std::endl;
	std::cout << "\tEngine version: " << appInfo.engineVersion << std::endl;
	std::cout << "\tAPI version: " << appInfo.apiVersion << std::endl;
	std::cout << std::endl;

	// Output all extensions being used by the instance
	std::cout << "Vulkan instance extensions:" << std::endl;
	// using  m_API.instanceCreateInfo.ppEnabledExtensionNames
	for (const auto& extension : m_API.extensions) { std::cout << "\t" << extension << std::endl; }
	std::cout << std::endl;

	// Output all validation layers being used by the instance
	std::cout << "Vulkan instance validation layers:" << std::endl;
	// using  m_API.instanceCreateInfo.ppEnabledLayerNames
	for (const auto& layer : m_API.validationLayers) { std::cout << "\t" << layer << std::endl; }
	std::cout << std::endl;
}

void VulkanInit::InitDevice()
{
	// Get the physical device
	m_API.deviceQueue.m_PhysicalDevice = m_API.instance.enumeratePhysicalDevices().front();
	InitSurface();

	vulkan::DeviceQueue& deviceQueue = m_API.deviceQueue;

	// Now set all the queues on the deviceQueue, to do this we need to loop through all the queue families
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_API.deviceQueue.m_PhysicalDevice.getQueueFamilyProperties();

	// Loop through all the queue families
	for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
	{
		// Get the queue family properties
		vk::QueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];

		// Check if the queue family supports graphics
		if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			if (!deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::GRAPHICS))
			{
				deviceQueue.SetQueueFamilyIndex(vulkan::QueueType::GRAPHICS, i);
				continue;
			}
		}

		// Check if the queue family supports presentation
		if (m_API.deviceQueue.m_PhysicalDevice.getSurfaceSupportKHR(i, m_API.window.m_Surface))
		{
			if (!deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::PRESENT))
			{
				deviceQueue.SetQueueFamilyIndex(vulkan::QueueType::PRESENT, i);
				continue;
			}
		}

		// Check if the queue family supports compute
		if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute)
		{
			if (!deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::COMPUTE))
			{
				deviceQueue.SetQueueFamilyIndex(vulkan::QueueType::COMPUTE, i);
				continue;
			}
		}

		// Check if the queue family supports transfer
		if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer)
		{
			if (!deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::TRANSFER))
			{
				deviceQueue.SetQueueFamilyIndex(vulkan::QueueType::TRANSFER, i);
				continue;
			}
		}
	}

	// Check if all the queues have been set
	if (!deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::GRAPHICS)) { throw std::runtime_error("No graphics queue family index found!"); }
	if (!deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::PRESENT)) { throw std::runtime_error("No present queue family index found!"); }

	// Get the queue priorities
	std::vector<float> queuePriorities = { 1.0f };

	// Create the queue create infos
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::GRAPHICS), static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data()));
	queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::PRESENT), static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data()));

	if (deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::TRANSFER))
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::TRANSFER), static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data()));

	if (deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::COMPUTE))
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::COMPUTE), static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data()));

	// Create the device create info
	vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo(vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data());

	// Set the device features
	vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
	deviceCreateInfo.setPEnabledFeatures(&deviceFeatures);

	// Set the device extensions
	if (m_API.deviceQueue.m_DeviceExtensions.empty())
	{
		// Add the minimum required extensions
		m_API.deviceQueue.m_DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_API.deviceQueue.m_DeviceExtensions.size()));
	deviceCreateInfo.setPpEnabledExtensionNames(m_API.deviceQueue.m_DeviceExtensions.data());

	// Create the device
	m_API.deviceQueue.m_Device = m_API.deviceQueue.m_PhysicalDevice.createDevice(deviceCreateInfo);

	// Get the queues
	deviceQueue.SetQueue(vulkan::QueueType::GRAPHICS, m_API.deviceQueue.m_Device.getQueue(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::GRAPHICS), 0));
	deviceQueue.SetQueue(vulkan::QueueType::PRESENT, m_API.deviceQueue.m_Device.getQueue(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::PRESENT), 0));

	if (deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::TRANSFER)) deviceQueue.SetQueue(vulkan::QueueType::TRANSFER, m_API.deviceQueue.m_Device.getQueue(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::TRANSFER), 0));

	if (deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::COMPUTE)) deviceQueue.SetQueue(vulkan::QueueType::COMPUTE, m_API.deviceQueue.m_Device.getQueue(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::COMPUTE), 0));

	// Set the device queue
	m_API.deviceQueue = deviceQueue;

	// Output all the device extensions
	std::cout << "Device Extensions:" << std::endl;
	for (const auto& extension : m_API.deviceQueue.m_DeviceExtensions) { std::cout << "\t" << extension << std::endl; }
	std::cout << std::endl;

	vk::PhysicalDeviceProperties properties;
	m_API.deviceQueue.m_PhysicalDevice.getProperties(&properties);

	uint32_t version = properties.apiVersion;
	uint32_t major   = VK_VERSION_MAJOR(version);
	uint32_t minor   = VK_VERSION_MINOR(version);
	uint32_t patch   = VK_VERSION_PATCH(version);

	// output device info
	std::cout << "Device Info:" << std::endl;
	std::cout << "\tVulkan version: " << major << "." << minor << "." << patch << std::endl;
	std::cout << "\tName: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().deviceName << std::endl;
	std::cout << "\tType: " << vk::to_string(m_API.deviceQueue.m_PhysicalDevice.getProperties().deviceType) << std::endl;
	std::cout << "\tAPI Version: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().apiVersion << std::endl;
	std::cout << "\tDriver Version: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().driverVersion << std::endl;
	std::cout << "\tVendor ID: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().vendorID << std::endl;
	std::cout << "\tDevice ID: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().deviceID << std::endl;
	std::cout << "\tQueue Family Count: " << m_API.deviceQueue.m_PhysicalDevice.getQueueFamilyProperties().size() << std::endl;
	std::cout << "\tMinimum alignment for uniform buffers: " << deviceQueue.m_PhysicalDevice.getProperties().limits.minUniformBufferOffsetAlignment << std::endl;
	std::cout << std::endl;
}

void VulkanInit::InitSurface()
{
	// Create the surface using glfw
	VkSurfaceKHR tmpSurface = {};
	if (glfwCreateWindowSurface(m_API.instance, m_API.window.GetWindow(), nullptr, &tmpSurface) != VK_SUCCESS) { std::runtime_error("failed ot create window surface!"); }

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
	if (window.m_SurfaceCapabilities.maxImageCount > 0 && window.m_SurfaceImageCount > window.m_SurfaceCapabilities.maxImageCount) { window.m_SurfaceImageCount = window.m_SurfaceCapabilities.maxImageCount; }

	// Output the surface properties
	std::cout << "Vulkan surface properties:" << std::endl;
	std::cout << "\tSurface Format: " << vk::to_string(window.m_SurfaceFormat.format) << std::endl;
	std::cout << "\tSurface Color Space: " << vk::to_string(window.m_SurfaceFormat.colorSpace) << std::endl;
	std::cout << "\tSurface Present Mode: " << vk::to_string(window.m_SurfacePresentMode) << std::endl;
	std::cout << "\tSurface Image Count: " << window.m_SurfaceImageCount << std::endl;
	std::cout << "\tSurface Extent: " << window.m_SurfaceExtent.width << "x" << window.m_SurfaceExtent.height << std::endl;
	std::cout << "\tSurface Min Image Count: " << window.m_SurfaceCapabilities.minImageCount << std::endl;
	std::cout << "\tSurface Max Image Count: " << window.m_SurfaceCapabilities.maxImageCount << std::endl;
	std::cout << "\tSurface Current Extent: " << window.m_SurfaceCapabilities.currentExtent.width << "x" << window.m_SurfaceCapabilities.currentExtent.height << std::endl;
	std::cout << std::endl;
}