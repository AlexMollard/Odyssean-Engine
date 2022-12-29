#include "pch.h"

#include "VulkanEngine.h"

#define GLFW_INCLUDE_VULKAN
#include "Vertex.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>

const uint32_t WIDTH                = 1920;
const uint32_t HEIGHT               = 1080;
const uint32_t MAX_FRAMES_IN_FLIGHT = 3;

VulkanEngine::~VulkanEngine()
{
	Cleanup();
}

float VulkanEngine::Update()
{
	glfwPollEvents();

	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		m_close = true;
		glfwSetWindowShouldClose(m_Window, true);
	}

	return 1;
}

void VulkanEngine::CreateInstance()
{
	std::cout << "-- CREATING INSTANCE --" << std::endl;

	// Check validation layers
	if (m_EnableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	// Application info
	vk::ApplicationInfo appInfo;
	appInfo.pApplicationName   = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName        = "No Engine";
	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_0;

	// Create info
	vk::InstanceCreateInfo createInfo;
	createInfo.pApplicationInfo = &appInfo;

	// Get extensions
	auto extensions                    = GetRequiredExtensions();
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	// Validation layers
	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (m_EnableValidationLayers)
	{
		createInfo.enabledLayerCount   = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	// Create instance
	if (vk::createInstance(&createInfo, nullptr, &m_Instance) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create instance!");
	}
}

void VulkanEngine::SetupDebugMessenger()
{
	std::cout << "-- SETTING UP DEBUG MESSENGER --" << std::endl;
	if (!m_EnableValidationLayers)
		return;

	vk::DebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr, &m_DebugMessenger) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

void VulkanEngine::CreateSurface()
{
	std::cout << "-- CREATING SURFACE --" << std::endl;
	// Cast to VkSurfaceKHR for GLFW to understand
	auto vkSurface = VkSurfaceKHR(m_Surface);
	if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &vkSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}
	// Remember to cast back to vk::SurfaceKHR so vulkan keeps the handle
	m_Surface = vk::SurfaceKHR(vkSurface);
}

void VulkanEngine::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	m_Instance.enumeratePhysicalDevices(&deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	std::vector<vk::PhysicalDevice> devices(deviceCount);
	m_Instance.enumeratePhysicalDevices(&deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			m_PhysicalDevice = device;
			break;
		}
	}

	if (m_PhysicalDevice == vk::PhysicalDevice(nullptr))
	{
		throw std::runtime_error("Failed to find a suitable GPU!");
	}

	// Output device name
	vk::PhysicalDeviceProperties deviceProperties;
	m_PhysicalDevice.getProperties(&deviceProperties);
	std::cout << "-- PICKING DEVICE --" << std::endl;
	std::cout << "Using device: " << deviceProperties.deviceName << std::endl;
	std::cout << "Device type: " << vk::to_string(deviceProperties.deviceType) << std::endl;
	std::cout << "API version: " << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "." << VK_VERSION_MINOR(deviceProperties.apiVersion) << "." << VK_VERSION_PATCH(deviceProperties.apiVersion) << std::endl;
	std::cout << "Driver version: " << VK_VERSION_MAJOR(deviceProperties.driverVersion) << "." << VK_VERSION_MINOR(deviceProperties.driverVersion) << "." << VK_VERSION_PATCH(deviceProperties.driverVersion) << std::endl;
	std::cout << "Vendor ID: " << deviceProperties.vendorID << std::endl;
	std::cout << "Device ID: " << deviceProperties.deviceID << std::endl;
}

void VulkanEngine::CreateLogicalDevice()
{
	std::cout << "-- CREATING LOGICAL DEVICE --" << std::endl;
	QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		vk::DeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	vk::PhysicalDeviceFeatures deviceFeatures;

	vk::DeviceCreateInfo createInfo;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos    = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount   = static_cast<uint32_t>(m_DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (m_EnableValidationLayers)
	{
		createInfo.enabledLayerCount   = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (m_PhysicalDevice.createDevice(&createInfo, nullptr, &m_Device) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	m_Device.getQueue(indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	m_Device.getQueue(indices.presentFamily.value(), 0, &m_PresentQueue);
}

void VulkanEngine::CreateSwapChain()
{
	std::cout << "-- CREATING SWAP CHAIN --" << std::endl;
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

	vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode     = ChooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent                = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.surface = m_Surface;

	createInfo.minImageCount    = imageCount;
	createInfo.imageFormat      = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent      = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage       = vk::ImageUsageFlagBits::eColorAttachment;

	QueueFamilyIndices indices    = FindQueueFamilies(m_PhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices   = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode      = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0;       // Optional
		createInfo.pQueueFamilyIndices   = nullptr; // Optional
	}

	createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode    = presentMode;
	createInfo.clipped        = VK_TRUE;

	createInfo.oldSwapchain = nullptr;

	if (m_Device.createSwapchainKHR(&createInfo, nullptr, &m_SwapChain) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}

	m_Device.getSwapchainImagesKHR(m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	m_Device.getSwapchainImagesKHR(m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent      = extent;
}

void VulkanEngine::CreateImageViews()
{
	std::cout << "-- CREATING IMAGE VIEWS --" << std::endl;
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		m_SwapChainImageViews[i] = CreateImageView(m_SwapChainImages[i], m_SwapChainImageFormat, vk::ImageAspectFlagBits::eColor, 1);

		// output image view
		std::cout << "ImageView " << i << ": " << m_SwapChainImageViews[i] << std::endl;
	}
}

bool VulkanEngine::CheckDeviceExtensionSupport(vk::PhysicalDevice device)
{
	uint32_t extensionCount;
	device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
	device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool VulkanEngine::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_ValidationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> VulkanEngine::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_EnableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

QueueFamilyIndices VulkanEngine::FindQueueFamilies(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

	std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
	device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = i;
		}

		vk::Bool32 presentSupport = false;
		device.getSurfaceSupportKHR(i, m_Surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

bool VulkanEngine::IsDeviceSuitable(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate                        = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	vk::PhysicalDeviceFeatures supportedFeatures;
	device.getFeatures(&supportedFeatures);

	return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

SwapChainSupportDetails VulkanEngine::QuerySwapChainSupport(vk::PhysicalDevice device)
{
	SwapChainSupportDetails details;

	device.getSurfaceCapabilitiesKHR(m_Surface, &details.capabilities);

	uint32_t formatCount;
	device.getSurfaceFormatsKHR(m_Surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		device.getSurfaceFormatsKHR(m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	device.getSurfacePresentModesKHR(m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		device.getSurfacePresentModesKHR(m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

vk::SurfaceFormatKHR VulkanEngine::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	std::cout << "-- ChooseSwapSurfaceFormat --" << std::endl;

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}

	// output all available formats
	for (const auto& availableFormat : availableFormats)
	{
		std::cout << "Available format: " << vk::to_string(availableFormat.format) << std::endl;
	}

	return availableFormats[0];
}

vk::PresentModeKHR VulkanEngine::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	std::cout << "-- ChooseSwapPresentMode --" << std::endl;
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
		{
			return availablePresentMode;
		}
	}

	// output all available present modes
	for (const auto& availablePresentMode : availablePresentModes)
	{
		std::cout << "Available present mode: " << vk::to_string(availablePresentMode) << std::endl;
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanEngine::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	std::cout << "-- ChooseSwapExtent --" << std::endl;
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);

		vk::Extent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width  = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngine::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

std::vector<char> VulkanEngine::ReadFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void VulkanEngine::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& debugCreateInfo)
{
	debugCreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	debugCreateInfo.messageType     = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
	debugCreateInfo.pfnUserCallback = DebugCallback;
	debugCreateInfo.pUserData       = nullptr; // Optional
}

vk::Result VulkanEngine::CreateDebugUtilsMessengerEXT(const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return static_cast<vk::Result>(
			func(m_Instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(pCreateInfo), reinterpret_cast<const VkAllocationCallbacks*>(pAllocator), reinterpret_cast<VkDebugUtilsMessengerEXT*>(pDebugMessenger)));
	}
	else
	{
		return vk::Result::eErrorExtensionNotPresent;
	}
}

void VulkanEngine::DestroyDebugUtilsMessengerEXT(vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(m_Instance, static_cast<VkDebugUtilsMessengerEXT>(debugMessenger), reinterpret_cast<const VkAllocationCallbacks*>(pAllocator));
	}
}

vk::ShaderModule VulkanEngine::CreateShaderModule(std::vector<char> vertShaderCode)
{
	vk::ShaderModuleCreateInfo createInfo;
	createInfo.codeSize = vertShaderCode.size();
	createInfo.pCode    = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

	vk::ShaderModule shaderModule;
	if (m_Device.createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

vk::ImageView VulkanEngine::CreateImageView(vk::Image m_SwapChainImage, vk::Format m_SwapChainImageFormat, vk::ImageAspectFlagBits param3, int param4)
{
	vk::ImageViewCreateInfo viewInfo;
	viewInfo.image                           = m_SwapChainImage;
	viewInfo.viewType                        = vk::ImageViewType::e2D;
	viewInfo.format                          = m_SwapChainImageFormat;
	viewInfo.subresourceRange.aspectMask     = param3;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = 1;

	return m_Device.createImageView(viewInfo);
}

vk::Format VulkanEngine::FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for (vk::Format format : candidates)
	{
		vk::FormatProperties props = m_PhysicalDevice.getFormatProperties(format);

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

vk::Format VulkanEngine::FindDepthFormat()
{
	//VK_FORMAT_D16_UNORM
	return FindSupportedFormat({ vk::Format::eD16Unorm, vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

void VulkanEngine::CreateRenderPass()
{
	std::cout << "-- CREATING RENDER PASS --" << std::endl;
	vk::AttachmentDescription colorAttachment;
	colorAttachment.format         = m_SwapChainImageFormat;
	colorAttachment.samples        = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout  = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

	// Color attachment reference
	vk::AttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

	// Depth attachment
	vk::AttachmentDescription depthAttachment;
	depthAttachment.format         = FindDepthFormat();
	depthAttachment.samples        = vk::SampleCountFlagBits::e1;
	depthAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
	depthAttachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
	depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.initialLayout  = vk::ImageLayout::eUndefined;
	depthAttachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	// Depth attachment reference
	vk::AttachmentReference depthAttachmentRef;
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	// Subpass
	vk::SubpassDescription subpass;
	subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount    = 1;
	subpass.pColorAttachments       = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	// Subpass dependency
	vk::SubpassDependency dependency;
	dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass    = 0;
	dependency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.srcAccessMask = vk::AccessFlags();
	dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	// Render pass info
	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount          = 2;
	vk::AttachmentDescription attachments[] = { colorAttachment, depthAttachment };
	renderPassInfo.pAttachments             = attachments;
	renderPassInfo.subpassCount             = 1;
	renderPassInfo.pSubpasses               = &subpass;
	renderPassInfo.dependencyCount          = 1;
	renderPassInfo.pDependencies            = &dependency;

	if (m_Device.createRenderPass(&renderPassInfo, nullptr, &m_RenderPass) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create render pass!");
	}

	// output render pass
	std::cout << "RenderPass: " << m_RenderPass << std::endl;
}

void VulkanEngine::CreateDescriptorSetLayout()
{
	std::cout << "-- CREATING DESCRIPTOR SET LAYOUT --" << std::endl;
	vk::DescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding            = 0;
	uboLayoutBinding.descriptorType     = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.descriptorCount    = 1;
	uboLayoutBinding.stageFlags         = vk::ShaderStageFlagBits::eVertex;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings    = &uboLayoutBinding;

	if (m_Device.createDescriptorSetLayout(&layoutInfo, nullptr, &m_DescriptorSetLayout) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create descriptor set layout!");
	}

	// output descriptor set layout
	std::cout << "DescriptorSetLayout: " << m_DescriptorSetLayout << std::endl;

	// Create the descriptor pool
	std::cout << "-- CREATING DESCRIPTOR POOL --" << std::endl;
	vk::DescriptorPoolSize poolSize;
	poolSize.type            = vk::DescriptorType::eUniformBuffer;
	poolSize.descriptorCount = 1;

	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes    = &poolSize;
	poolInfo.maxSets       = 1;

	if (m_Device.createDescriptorPool(&poolInfo, nullptr, &m_DescriptorPool) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create descriptor pool!");
	}

	// output descriptor pool
	std::cout << "DescriptorPool: " << m_DescriptorPool << std::endl;

	// Create the descriptor sets
	std::cout << "-- CREATING DESCRIPTOR SETS --" << std::endl;
	vk::DescriptorSetLayout layouts[] = { m_DescriptorSetLayout };
	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.descriptorPool     = m_DescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts        = layouts;

	m_DescriptorSets = m_Device.allocateDescriptorSets(allocInfo);
}

void VulkanEngine::CreateDepthResources()
{
	std::cout << "Create depth image view" << std::endl;
	// Get the surface data
	vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(QuerySwapChainSupport(m_PhysicalDevice).formats);

	const vk::Format depthFormat          = vk::Format::eD16Unorm;
	vk::FormatProperties formatProperties = m_PhysicalDevice.getFormatProperties(depthFormat);

	vk::ImageTiling tiling;
	if (formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
	{
		tiling = vk::ImageTiling::eLinear;
	}
	else if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
	{
		tiling = vk::ImageTiling::eOptimal;
	}
	else
	{
		throw std::runtime_error("DepthStencilAttachment is not supported for D16Unorm depth format.");
	}

	vk::ImageCreateInfo imageInfo;
	imageInfo.imageType     = vk::ImageType::e2D;
	imageInfo.format        = depthFormat;
	imageInfo.extent.width  = m_SwapChainExtent.width;
	imageInfo.extent.height = m_SwapChainExtent.height;
	imageInfo.extent.depth  = 1;
	imageInfo.mipLevels     = 1;
	imageInfo.arrayLayers   = 1;
	imageInfo.samples       = vk::SampleCountFlagBits::e1;
	imageInfo.tiling        = tiling;
	imageInfo.usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	imageInfo.sharingMode   = vk::SharingMode::eExclusive;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;

	m_DepthImage = m_Device.createImage(imageInfo);

	vk::PhysicalDeviceMemoryProperties memoryProperties = m_PhysicalDevice.getMemoryProperties();
	vk::MemoryRequirements memoryRequirements           = m_Device.getImageMemoryRequirements(m_DepthImage);
	uint32_t typeBits                                   = memoryRequirements.memoryTypeBits;
	uint32_t typeIndex                                  = uint32_t(~0);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal))
		{
			typeIndex = i;
			break;
		}
		typeBits >>= 1;
	}
	assert(typeIndex != uint32_t(~0));
	m_DepthImageMemory = m_Device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, typeIndex));

	m_Device.bindImageMemory(m_DepthImage, m_DepthImageMemory, 0);

	m_DepthImageView = m_Device.createImageView(vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), m_DepthImage, vk::ImageViewType::e2D, depthFormat, {}, { vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1 }));
}

vk::MemoryAllocateInfo VulkanEngine::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = m_PhysicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return vk::MemoryAllocateInfo(memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size, i);
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

vk::Buffer VulkanEngine::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
{
	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size        = size;
	bufferInfo.usage       = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	vk::Buffer buffer = m_Device.createBuffer(bufferInfo);

	vk::MemoryRequirements memRequirements = m_Device.getBufferMemoryRequirements(buffer);

	// Make sure the buffer is suitable for the memory type
	assert((memRequirements.memoryTypeBits & 1) == 1);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = 0;

	vk::DeviceMemory bufferMemory = m_Device.allocateMemory(allocInfo);

	m_Device.bindBufferMemory(buffer, bufferMemory, 0);

	// Set VertexBufferMemoryRequirements or IndexBufferMemoryRequirements if this is a vertex or index buffer
	if (usage == vk::BufferUsageFlagBits::eVertexBuffer)
	{
		m_VertexBufferMemoryRequirements = memRequirements;
	}
	else if (usage == vk::BufferUsageFlagBits::eIndexBuffer)
	{
		m_IndexBufferMemoryRequirements = memRequirements;
	}

	return buffer;
}

void VulkanEngine::CreateGraphicsPipeline()
{
	std::cout << "-- CREATING GRAPHICS PIPELINE --" << std::endl;
	auto vertShaderCode = ReadFile("../Resources/Shaders/vert.spv");
	auto fragShaderCode = ReadFile("../Resources/Shaders/frag.spv");

	vk::ShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	vk::ShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
	vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName  = "main";

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
	fragShaderStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName  = "main";

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription    = Vertex::GetBindingDescription();
	auto attributeDescriptions = Vertex::GetAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.vertexBindingDescriptionCount   = 1;
	vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.topology               = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	vk::Viewport viewport;
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = (float)m_SwapChainExtent.width;
	viewport.height   = (float)m_SwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vk::Rect2D scissor;
	//scissor.offset = { 0, 0 };
	scissor.extent = m_SwapChainExtent;

	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.viewportCount = 1;
	viewportState.pViewports    = &viewport;
	viewportState.scissorCount  = 1;
	viewportState.pScissors     = &scissor;

	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.depthClampEnable        = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode             = vk::PolygonMode::eFill;
	rasterizer.lineWidth               = 1.0f;
	rasterizer.cullMode                = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace               = vk::FrontFace::eClockwise;
	rasterizer.depthBiasEnable         = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp          = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional

	// depth testing
	vk::PipelineDepthStencilStateCreateInfo depthStencil;
	depthStencil.depthTestEnable       = VK_TRUE;
	depthStencil.depthWriteEnable      = VK_TRUE;
	depthStencil.depthCompareOp        = vk::CompareOp::eLess;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds        = 0.0f; // Optional
	depthStencil.maxDepthBounds        = 1.0f; // Optional
	depthStencil.stencilTestEnable     = VK_FALSE;

	vk::PipelineMultisampleStateCreateInfo multisampling;
	multisampling.sampleShadingEnable   = VK_FALSE;
	multisampling.rasterizationSamples  = vk::SampleCountFlagBits::e1;
	multisampling.minSampleShading      = 1.0f;     // Optional
	multisampling.pSampleMask           = nullptr;  // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable      = VK_FALSE; // Optional

	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable         = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;  // Optional
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.colorBlendOp        = vk::BlendOp::eAdd;      // Optional
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;  // Optional
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.alphaBlendOp        = vk::BlendOp::eAdd;      // Optional

	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.logicOpEnable     = VK_FALSE;
	colorBlending.logicOp           = vk::LogicOp::eCopy; // Optional
	colorBlending.attachmentCount   = 1;
	colorBlending.pAttachments      = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setLayoutCount         = 1;
	pipelineLayoutInfo.pSetLayouts            = &m_DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;       // Optional
	pipelineLayoutInfo.pPushConstantRanges    = nullptr; // Optional

	if (m_Device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &m_PipelineLayout) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.stageCount          = static_cast<uint32_t>(2);
	pipelineInfo.pStages             = shaderStages;
	pipelineInfo.pVertexInputState   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState      = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState   = &multisampling;
	pipelineInfo.pDepthStencilState  = &depthStencil;
	pipelineInfo.pColorBlendState    = &colorBlending;
	pipelineInfo.pDynamicState       = nullptr; // Optional
	pipelineInfo.layout              = m_PipelineLayout;
	pipelineInfo.renderPass          = m_RenderPass;
	pipelineInfo.subpass             = 0;
	pipelineInfo.basePipelineHandle  = nullptr; // Optional
	pipelineInfo.basePipelineIndex   = -1;      // Optional

	if (m_Device.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	m_Device.destroyShaderModule(vertShaderModule);
	m_Device.destroyShaderModule(fragShaderModule);

	// Output pipeline info
	std::cout << "Pipeline created" << std::endl;
}

void VulkanEngine::CreateFramebuffers()
{
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
	{
		std::array<vk::ImageView, 2> attachments = { m_SwapChainImageViews[i], m_DepthImageView };

		vk::FramebufferCreateInfo framebufferInfo;
		framebufferInfo.renderPass      = m_RenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments    = attachments.data();
		framebufferInfo.width           = m_SwapChainExtent.width;
		framebufferInfo.height          = m_SwapChainExtent.height;
		framebufferInfo.layers          = 1;

		m_SwapChainFramebuffers[i] = m_Device.createFramebuffer(framebufferInfo);
	}

	// Output framebuffer info
	std::cout << "-- Framebuffers created -- " << std::endl;
	std::cout << "Number of framebuffers: " << m_SwapChainFramebuffers.size() << std::endl;
	std::cout << "Framebuffer width: " << m_SwapChainExtent.width << std::endl;
	std::cout << "Framebuffer height: " << m_SwapChainExtent.height << std::endl;
	std::cout << "Framebuffer layers: " << 1 << std::endl;
}

void VulkanEngine::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice);

	vk::CommandPoolCreateInfo poolInfo;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer; // Optional

	if (m_Device.createCommandPool(&poolInfo, nullptr, &m_CommandPool) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create command pool!");
	}

	// Output command pool info
	std::cout << "-- Command pool created -- " << std::endl;
	std::cout << "Command pool queue family index: " << queueFamilyIndices.graphicsFamily.value() << std::endl;
}

void VulkanEngine::CreateCommandBuffers()
{
	m_CommandBuffers.resize(m_SwapChainFramebuffers.size());

	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool        = m_CommandPool;
	allocInfo.level              = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (m_Device.allocateCommandBuffers(&allocInfo, m_CommandBuffers.data()) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	// Output command buffer info
	std::cout << "-- Command buffers created -- " << std::endl;
	std::cout << "Number of command buffers: " << m_CommandBuffers.size() << std::endl;
}

void VulkanEngine::CreateSyncObjects()
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	m_ImagesInFlight.resize(m_SwapChainImages.size(), nullptr);

	vk::SemaphoreCreateInfo semaphoreInfo;
	vk::FenceCreateInfo fenceInfo;
	fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (m_Device.createSemaphore(&semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != vk::Result::eSuccess || m_Device.createSemaphore(&semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != vk::Result::eSuccess ||
			m_Device.createFence(&fenceInfo, nullptr, &m_InFlightFences[i]) != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

	// Output sync object info
	std::cout << "-- Sync objects created -- " << std::endl;
	std::cout << "Number of image available semaphores: " << m_ImageAvailableSemaphores.size() << std::endl;
	std::cout << "Number of render finished semaphores: " << m_RenderFinishedSemaphores.size() << std::endl;
	std::cout << "Number of in flight fences: " << m_InFlightFences.size() << std::endl;
}

void VulkanEngine::CleanupSwapChain()
{
	for (auto framebuffer : m_SwapChainFramebuffers)
	{
		m_Device.destroyFramebuffer(framebuffer, nullptr);
	}

	m_Device.freeCommandBuffers(m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());

	m_Device.destroyPipeline(m_GraphicsPipeline, nullptr);
	m_Device.destroyPipelineLayout(m_PipelineLayout, nullptr);
	m_Device.destroyRenderPass(m_RenderPass, nullptr);

	for (auto imageView : m_SwapChainImageViews)
	{
		m_Device.destroyImageView(imageView, nullptr);
	}

	m_Device.destroySwapchainKHR(m_SwapChain, nullptr);
}

void VulkanEngine::Cleanup()
{
	CleanupSwapChain();

	m_Device.destroyDescriptorSetLayout(m_DescriptorSetLayout, nullptr);

	//m_Device.destroyBuffer(m_VertexBuffer, nullptr);
	//m_Device.freeMemory(m_VertexBufferMemory, nullptr);

	//m_Device.destroyBuffer(m_IndexBuffer, nullptr);
	//m_Device.freeMemory(m_IndexBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_Device.destroySemaphore(m_RenderFinishedSemaphores[i], nullptr);
		m_Device.destroySemaphore(m_ImageAvailableSemaphores[i], nullptr);
		m_Device.destroyFence(m_InFlightFences[i], nullptr);
	}

	m_Device.destroyCommandPool(m_CommandPool, nullptr);

	// Destroy depth texture
	m_Device.destroyImageView(m_DepthImageView, nullptr);
	m_Device.destroyImage(m_DepthImage, nullptr);
	m_Device.freeMemory(m_DepthImageMemory, nullptr);

	if (m_EnableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr);
	}

	m_Device.destroy(nullptr);

	m_Instance.destroySurfaceKHR(m_Surface, nullptr);
	m_Instance.destroy(nullptr);

	glfwDestroyWindow(m_Window);

	glfwTerminate();
}

void VulkanEngine::RecreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_Window, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(m_Window, &width, &height);
		glfwWaitEvents();
	}

	m_Device.waitIdle();

	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateDepthResources();
	CreateFramebuffers();
	CreateCommandBuffers();
}

void VulkanEngine::Init(const char* windowName, int width, int height)
{
	m_close = false;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);

	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();

	CreateDescriptorSetLayout();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateDepthResources();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	CreateSyncObjects();

	// Create a vertex and index buffer
	CreateMeshBuffer();
}

void VulkanEngine::CreateMeshBuffer()
{
	// Create the quad vertices
	float vertices[] = { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f };

	// Create the quad indices
	uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

	// Calculate the vertex and index buffer sizes
	m_VertexBufferSize = sizeof(float) * 5 * 4;
	m_IndexBufferSize  = sizeof(uint32_t) * 6;

	// Create a vertex buffer
	m_VertexBuffer = CreateBuffer(m_VertexBufferSize, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	// Create an index buffer
	m_IndexBuffer = CreateBuffer(m_IndexBufferSize, vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	// Set the m_vertexBufferMemory and m_indexBufferMemory variables
	m_Device.getBufferMemoryRequirements(m_VertexBuffer, &m_VertexBufferMemoryRequirements);
	m_Device.getBufferMemoryRequirements(m_IndexBuffer, &m_IndexBufferMemoryRequirements);

	m_VertexBufferMemory = vk::DeviceMemory();
	m_IndexBufferMemory = vk::DeviceMemory();

	// Lambda function to find a memory type
	auto FindMemoryType = [&](uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties memoryProperties = m_PhysicalDevice.getMemoryProperties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	};

	// Allocate memory for the vertex buffer
	vk::MemoryAllocateInfo vertexBufferMemoryAllocateInfo = vk::MemoryAllocateInfo();
	vertexBufferMemoryAllocateInfo.setAllocationSize(m_VertexBufferMemoryRequirements.size);
	vertexBufferMemoryAllocateInfo.setMemoryTypeIndex(FindMemoryType(m_VertexBufferMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
	m_VertexBufferMemory = m_Device.allocateMemory(vertexBufferMemoryAllocateInfo);

	// Allocate memory for the index buffer
	vk::MemoryAllocateInfo indexBufferMemoryAllocateInfo = vk::MemoryAllocateInfo();
	indexBufferMemoryAllocateInfo.setAllocationSize(m_IndexBufferMemoryRequirements.size);
	indexBufferMemoryAllocateInfo.setMemoryTypeIndex(FindMemoryType(m_IndexBufferMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
	m_IndexBufferMemory = m_Device.allocateMemory(indexBufferMemoryAllocateInfo);

	// Copy the vertex data to the vertex buffer
	uint8_t* pData = static_cast<uint8_t*>(m_Device.mapMemory(m_VertexBufferMemory, 0, m_VertexBufferSize, vk::MemoryMapFlags()));
	memcpy(pData, vertices, (size_t)m_VertexBufferSize);
	m_Device.unmapMemory(m_VertexBufferMemory);

	// Copy the index data to the index buffer
	pData = static_cast<uint8_t*>(m_Device.mapMemory(m_IndexBufferMemory, 0, m_IndexBufferSize, vk::MemoryMapFlags()));
	memcpy(pData, indices, (size_t)m_IndexBufferSize);
	m_Device.unmapMemory(m_IndexBufferMemory);

	// now m_VertexBuffer and m_IndexBuffer are vk::Buffer objects and m_VertexBufferMemory and m_IndexBufferMemory are vk::DeviceMemory objects
	// that contain the vertex and index data respectively.
}

vk::CommandBuffer VulkanEngine::BeginSingleTimeCommands()
{
	// Create a command buffer
	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.level                         = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandPool                   = m_CommandPool;
	allocInfo.commandBufferCount            = 1;

	vk::CommandBuffer commandBuffer = m_Device.allocateCommandBuffers(allocInfo)[0];

	// Begin the command buffer
	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags                      = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void VulkanEngine::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	// End the command buffer
	commandBuffer.end();

	// Submit the command buffer
	vk::SubmitInfo submitInfo     = {};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;

	m_GraphicsQueue.submit(submitInfo, nullptr);
	m_GraphicsQueue.waitIdle();

	// Free the command buffer
	m_Device.freeCommandBuffers(m_CommandPool, commandBuffer);
}

// bind the vertex buffer to the command buffer
void VulkanEngine::BindVertexBuffer(vk::CommandBuffer commandBuffer)
{
	// Bind the vertex buffer
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, m_VertexBuffer, offsets);

	// Bind the index buffer
	commandBuffer.bindIndexBuffer(m_IndexBuffer, 0, vk::IndexType::eUint32);

	// Bind the descriptor sets
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_PipelineLayout, 0, m_DescriptorSets[m_CurrentFrame], nullptr);

	// Bind the pipeline
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_GraphicsPipeline);

	// Draw the quad
	commandBuffer.drawIndexed(6, 1, 0, 0, 0);

	// End the render pass
	commandBuffer.endRenderPass();

	// End the command buffer
	commandBuffer.end();
}

// This function will create a new command buffer and record the commands to draw the quad
void VulkanEngine::RecordCommandsToDrawQuad(vk::CommandBuffer commandBuffer)
{
	// Begin the render pass
	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass              = m_RenderPass;
	renderPassInfo.framebuffer             = m_SwapChainFramebuffers[m_CurrentFrame];
	renderPassInfo.renderArea.offset       = vk::Offset2D(0, 0);
	renderPassInfo.renderArea.extent       = m_SwapChainExtent;

	vk::ClearValue clearColor = {
		std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}
	};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues    = &clearColor;

	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	// Bind the graphics pipeline
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_GraphicsPipeline);

	// Bind the vertex buffer
	BindVertexBuffer(commandBuffer);

	// Draw the quad
	commandBuffer.drawIndexed(6, 1, 0, 0, 0);

	// End the render pass
	//commandBuffer.endRenderPass();
}

void VulkanEngine::Render()
{
	// Wait for the fence to be signaled
	m_Device.waitForFences(m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

	// Get the index of the next available swap chain image
	uint32_t imageIndex;
	vk::Result result = m_Device.acquireNextImageKHR(m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], nullptr, &imageIndex);

	// If the swap chain is out of date, recreate it
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (m_ImagesInFlight[imageIndex])
	{
		m_Device.waitForFences(m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	// Mark the image as now being in use by this frame
	m_ImagesInFlight[imageIndex] = m_InFlightFences[m_CurrentFrame];

	// Create a command buffer
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

	// Record the commands to draw the quad
	RecordCommandsToDrawQuad(commandBuffer);

	// End the command buffer
	EndSingleTimeCommands(commandBuffer);

	// Submit the command buffer
	vk::SubmitInfo submitInfo           = {};
	submitInfo.waitSemaphoreCount       = 1;
	submitInfo.pWaitSemaphores          = &m_ImageAvailableSemaphores[m_CurrentFrame];
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.pWaitDstStageMask        = waitStages;
	submitInfo.commandBufferCount       = 1;
	submitInfo.pCommandBuffers          = &commandBuffer;
	submitInfo.signalSemaphoreCount     = 1;
	submitInfo.pSignalSemaphores        = &m_RenderFinishedSemaphores[m_CurrentFrame];

	m_Device.resetFences(m_InFlightFences[m_CurrentFrame]);

	m_GraphicsQueue.submit(submitInfo, m_InFlightFences[m_CurrentFrame]);

	// Present the image
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = &m_RenderFinishedSemaphores[m_CurrentFrame];
	presentInfo.swapchainCount     = 1;
	presentInfo.pSwapchains        = &m_SwapChain;
	presentInfo.pImageIndices      = &imageIndex;

	result = m_PresentQueue.presentKHR(&presentInfo);

	// If the swap chain is out of date, recreate it
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_FramebufferResized)
	{
		m_FramebufferResized = false;
		RecreateSwapChain();
	}
	else if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	// Wait for the fence to be signaled
	m_Device.waitForFences(m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
}
