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
	InitSwapchain();
	InitRenderPass();
	InitFramebuffers();
	InitCommandPool();
	InitCommandBuffers();
	InitSyncObjects();
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
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

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
	m_API.physicalDevice = m_API.instance.enumeratePhysicalDevices().front();
	InitSurface();

	vulkan::DeviceQueue deviceQueue;

	// Now set all the queues on the deviceQueue, to do this we need to loop through all the queue families
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_API.physicalDevice.getQueueFamilyProperties();

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
		if (m_API.physicalDevice.getSurfaceSupportKHR(i, m_API.surface))
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
	//if (!deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::TRANSFER)) { throw std::runtime_error("No transfer queue family index found!"); } // Not required
	//if (!deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::COMPUTE)) { throw std::runtime_error("No compute queue family index found!"); } // Not required

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
	if (m_API.deviceExtensions.empty())
	{
		// Add the minimum required extensions
		m_API.deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_API.deviceExtensions.size()));
	deviceCreateInfo.setPpEnabledExtensionNames(m_API.deviceExtensions.data());

	// Create the device
	m_API.device = m_API.physicalDevice.createDevice(deviceCreateInfo);

	// Get the queues
	deviceQueue.SetQueue(vulkan::QueueType::GRAPHICS, m_API.device.getQueue(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::GRAPHICS), 0));
	deviceQueue.SetQueue(vulkan::QueueType::PRESENT, m_API.device.getQueue(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::PRESENT), 0));

	if (deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::TRANSFER)) 
		deviceQueue.SetQueue(vulkan::QueueType::TRANSFER, m_API.device.getQueue(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::TRANSFER), 0));

	if (deviceQueue.IsQueueFamilyIndexSet(vulkan::QueueType::COMPUTE)) 
		deviceQueue.SetQueue(vulkan::QueueType::COMPUTE, m_API.device.getQueue(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::COMPUTE), 0));

	// Set the device queue
	m_API.deviceQueue = deviceQueue;

	// Output all the device extensions
	std::cout << "Device Extensions:" << std::endl;
	for (const auto& extension : m_API.deviceExtensions) { std::cout << "\t" << extension << std::endl; }
	std::cout << std::endl;

	// output device info
	std::cout << "Device Info:" << std::endl;
	std::cout << "\tName: " << m_API.physicalDevice.getProperties().deviceName << std::endl;
	std::cout << "\tType: " << vk::to_string(m_API.physicalDevice.getProperties().deviceType) << std::endl;
	std::cout << "\tAPI Version: " << m_API.physicalDevice.getProperties().apiVersion << std::endl;
	std::cout << "\tDriver Version: " << m_API.physicalDevice.getProperties().driverVersion << std::endl;
	std::cout << "\tVendor ID: " << m_API.physicalDevice.getProperties().vendorID << std::endl;
	std::cout << "\tDevice ID: " << m_API.physicalDevice.getProperties().deviceID << std::endl;
	std::cout << "\tQueue Family Count: " << m_API.physicalDevice.getQueueFamilyProperties().size() << std::endl;
	std::cout << std::endl;
}

void VulkanInit::InitSurface()
{
	// Create the surface using glfw
	VkSurfaceKHR tmpSurface = {};
	if (glfwCreateWindowSurface(m_API.instance, m_API.window.GetWindow(), nullptr, &tmpSurface) != VK_SUCCESS) { std::runtime_error("failed ot create window surface!"); }

	m_API.surface = tmpSurface;

	// Get the surface capabilities
	m_API.surfaceCapabilities = m_API.physicalDevice.getSurfaceCapabilitiesKHR(m_API.surface);

	// Get the surface formats
	m_API.surfaceFormats = m_API.physicalDevice.getSurfaceFormatsKHR(m_API.surface);

	// Get the surface present modes
	m_API.surfacePresentModes = m_API.physicalDevice.getSurfacePresentModesKHR(m_API.surface);

	// Get the surface extent
	m_API.surfaceExtent = m_API.surfaceCapabilities.currentExtent;

	// Get the surface format
	m_API.surfaceFormat = m_API.surfaceFormats.front();

	// Get the surface present mode
	m_API.surfacePresentMode = vk::PresentModeKHR::eFifo;

	// Get the surface image count
	m_API.surfaceImageCount = m_API.surfaceCapabilities.minImageCount + 1;
	if (m_API.surfaceCapabilities.maxImageCount > 0 && m_API.surfaceImageCount > m_API.surfaceCapabilities.maxImageCount) { m_API.surfaceImageCount = m_API.surfaceCapabilities.maxImageCount; }

	// Output the surface properties
	std::cout << "Vulkan surface properties:" << std::endl;
	std::cout << "\tSurface extent: " << m_API.surfaceExtent.width << "x" << m_API.surfaceExtent.height << std::endl;
	std::cout << "\tSurface image count: " << m_API.surfaceImageCount << std::endl;
	std::cout << "\tSurface format: " << vk::to_string(m_API.surfaceFormat.format) << std::endl;
	std::cout << "\tSurface color space: " << vk::to_string(m_API.surfaceFormat.colorSpace) << std::endl;
	std::cout << "\tSurface present mode: " << vk::to_string(m_API.surfacePresentMode) << std::endl;
	std::cout << std::endl;
}

void VulkanInit::InitSwapchain()
{		
	// Create the swapchain create info
	vk::SwapchainCreateInfoKHR swapchainInfo;
	swapchainInfo.setSurface(m_API.surface);
	swapchainInfo.setMinImageCount(m_API.surfaceImageCount);
	swapchainInfo.setImageFormat(m_API.surfaceFormat.format);
	swapchainInfo.setImageColorSpace(m_API.surfaceFormat.colorSpace);
	swapchainInfo.setImageExtent(m_API.surfaceExtent);
	swapchainInfo.setImageArrayLayers(1);
	swapchainInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapchainInfo.setPreTransform(m_API.surfaceCapabilities.currentTransform);
	swapchainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapchainInfo.setPresentMode(m_API.surfacePresentMode);
	swapchainInfo.setClipped(VK_TRUE);
	swapchainInfo.setOldSwapchain(nullptr);

	// Get the queue family indices
	std::vector<uint32_t> queueFamilyIndices = { m_API.deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::GRAPHICS), m_API.deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::PRESENT) };

	// Check if the graphics and present queue family indices are the same
	if (queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		swapchainInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		swapchainInfo.setQueueFamilyIndexCount(2);
		swapchainInfo.setPQueueFamilyIndices(queueFamilyIndices.data());
	}
	else
	{
		swapchainInfo.setImageSharingMode(vk::SharingMode::eExclusive);
		swapchainInfo.setQueueFamilyIndexCount(0);
		swapchainInfo.setPQueueFamilyIndices(nullptr);
	}

	// Create the swapchain
	m_API.swapchain = m_API.device.createSwapchainKHR(swapchainInfo);

	// Get the swapchain images
	m_API.swapchainImages = m_API.device.getSwapchainImagesKHR(m_API.swapchain);

	// Create the swapchain image views
	m_API.swapchainImageViews.resize(m_API.swapchainImages.size());

	for (size_t i = 0; i < m_API.swapchainImages.size(); i++)
	{
		vk::ImageViewCreateInfo imageViewInfo;
		imageViewInfo.setImage(m_API.swapchainImages[i]);
		imageViewInfo.setViewType(vk::ImageViewType::e2D);
		imageViewInfo.setFormat(m_API.surfaceFormat.format);
		imageViewInfo.setComponents({ vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity });
		imageViewInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

		m_API.swapchainImageViews[i] = m_API.device.createImageView(imageViewInfo);
	}

	vulkan::SwapchainInfo swapchain;

	// Create the swapchain info
	swapchain.setSwapchain(m_API.swapchain);
	swapchain.setExtent(m_API.surfaceExtent);
	swapchain.setFormat(m_API.surfaceFormat.format);
	swapchain.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapchain.setPreTransform(m_API.surfaceCapabilities.currentTransform);
	swapchain.setPresentMode(m_API.surfacePresentMode);
	swapchain.setImages(m_API.swapchainImages);
	swapchain.setImageCount(m_API.surfaceImageCount);

	m_API.swapchainInfo = swapchain;
	
	// Output the swapchain info properties
	std::cout << "Vulkan swapchain image properties:" << std::endl;
	std::cout << "\tCount: " << m_API.surfaceImageCount << std::endl;
	std::cout << "\tFormat: " << vk::to_string(m_API.surfaceFormat.format) << std::endl;
	std::cout << "\tColor space: " << vk::to_string(m_API.surfaceFormat.colorSpace) << std::endl;
	std::cout << "\tExtent: " << m_API.surfaceExtent.width << "x" << m_API.surfaceExtent.height << std::endl;
	std::cout << "\tUsage: " << vk::to_string(vk::ImageUsageFlagBits::eColorAttachment) << std::endl;
	std::cout << "\tPre-transform: " << vk::to_string(m_API.surfaceCapabilities.currentTransform) << std::endl;
	std::cout << "\tPresent mode: " << vk::to_string(m_API.surfacePresentMode) << std::endl;
	std::cout << std::endl;
}

// Finish this function
void VulkanInit::InitRenderPass()
{
	vulkan::RenderPassFramebuffer renderPassFramebuffer;
	// Contains:
	//	vk::RenderPass  m_RenderPass;
	//	vk::Framebuffer m_Framebuffer;
	
}

void VulkanInit::InitFramebuffers()
{}

void VulkanInit::InitCommandPool()
{}

void VulkanInit::InitCommandBuffers()
{}

void VulkanInit::InitSyncObjects()
{}
