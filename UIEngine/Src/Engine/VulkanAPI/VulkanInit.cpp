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

	// output device info
	std::cout << "Device Info:" << std::endl;
	std::cout << "\tName: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().deviceName << std::endl;
	std::cout << "\tType: " << vk::to_string(m_API.deviceQueue.m_PhysicalDevice.getProperties().deviceType) << std::endl;
	std::cout << "\tAPI Version: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().apiVersion << std::endl;
	std::cout << "\tDriver Version: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().driverVersion << std::endl;
	std::cout << "\tVendor ID: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().vendorID << std::endl;
	std::cout << "\tDevice ID: " << m_API.deviceQueue.m_PhysicalDevice.getProperties().deviceID << std::endl;
	std::cout << "\tQueue Family Count: " << m_API.deviceQueue.m_PhysicalDevice.getQueueFamilyProperties().size() << std::endl;
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

void VulkanInit::InitSwapchain()
{
	Window& window = m_API.window;
	
	// Create the swapchain create info
	vk::SwapchainCreateInfoKHR swapchainInfo;
	swapchainInfo.setSurface(window.m_Surface);
	swapchainInfo.setMinImageCount(window.m_SurfaceImageCount);
	swapchainInfo.setImageFormat(window.m_SurfaceFormat.format);
	swapchainInfo.setImageColorSpace(window.m_SurfaceFormat.colorSpace);
	swapchainInfo.setImageExtent(window.m_SurfaceExtent);
	swapchainInfo.setImageArrayLayers(1);
	swapchainInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapchainInfo.setPreTransform(window.m_SurfaceCapabilities.currentTransform);
	swapchainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapchainInfo.setPresentMode(window.m_SurfacePresentMode);
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
	m_API.swapchainInfo.m_Swapchain = m_API.deviceQueue.m_Device.createSwapchainKHR(swapchainInfo);

	// Get the swapchain images
	m_API.swapchainInfo.m_Images = m_API.deviceQueue.m_Device.getSwapchainImagesKHR(m_API.swapchainInfo.m_Swapchain);

	// Create the swapchain image views
	m_API.swapchainInfo.m_ImageViews.resize(m_API.swapchainInfo.m_Images.size());

	for (size_t i = 0; i < m_API.swapchainInfo.m_Images.size(); i++)
	{
		vk::ImageViewCreateInfo imageViewInfo;
		imageViewInfo.setImage(m_API.swapchainInfo.m_Images[i]);
		imageViewInfo.setViewType(vk::ImageViewType::e2D);
		imageViewInfo.setFormat(window.m_SurfaceFormat.format);
		imageViewInfo.setComponents({ vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity });
		imageViewInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

		m_API.swapchainInfo.m_ImageViews[i] = m_API.deviceQueue.m_Device.createImageView(imageViewInfo);
	}

	m_API.swapchainInfo.m_Extent       = window.m_SurfaceExtent;
	m_API.swapchainInfo.m_Format       = window.m_SurfaceFormat.format;
	m_API.swapchainInfo.m_PresentMode  = window.m_SurfacePresentMode;
	m_API.swapchainInfo.m_ImageCount   = window.m_SurfaceImageCount;
	m_API.swapchainInfo.m_PreTransform = window.m_SurfaceCapabilities.currentTransform;
	m_API.swapchainInfo.m_ImageUsage   = vk::ImageUsageFlagBits::eColorAttachment;

	// Output the swapchain info properties
	std::cout << "Vulkan swapchain image properties:" << std::endl;
	std::cout << "  Image count: " << m_API.swapchainInfo.m_ImageCount << std::endl;
	std::cout << "  Image format: " << vk::to_string(m_API.swapchainInfo.m_Format) << std::endl;
	std::cout << "  Image extent: " << m_API.swapchainInfo.m_Extent.width << "x" << m_API.swapchainInfo.m_Extent.height << std::endl;
	std::cout << "  Image present mode: " << vk::to_string(m_API.swapchainInfo.m_PresentMode) << std::endl;
	std::cout << "  Image usage: " << vk::to_string(m_API.swapchainInfo.m_ImageUsage) << std::endl;
	std::cout << "  Image pre-transform: " << vk::to_string(m_API.swapchainInfo.m_PreTransform) << std::endl;
	std::cout << std::endl;
}

// Finish this function
void VulkanInit::InitRenderPass()
{
	// Contains:
	//	vk::RenderPass  m_RenderPass;
	//	vk::Framebuffer m_Framebuffer;

	// Create the render pass
	vk::AttachmentDescription colorAttachment;
	colorAttachment.setFormat(m_API.swapchainInfo.m_Format);
	colorAttachment.setSamples(vk::SampleCountFlagBits::e1);
	colorAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
	colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
	colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
	colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference colorAttachmentRef;
	colorAttachmentRef.setAttachment(0);
	colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription subpass;
	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
	subpass.setColorAttachmentCount(1);
	subpass.setPColorAttachments(&colorAttachmentRef);

	vk::SubpassDependency dependency;
	dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
	dependency.setDstSubpass(0);
	dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	dependency.setSrcAccessMask({});
	dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.setAttachmentCount(1);
	renderPassInfo.setPAttachments(&colorAttachment);
	renderPassInfo.setSubpassCount(1);
	renderPassInfo.setPSubpasses(&subpass);
	renderPassInfo.setDependencyCount(1);
	renderPassInfo.setPDependencies(&dependency);

	m_API.renderPassFrameBuffers.m_RenderPass = m_API.deviceQueue.m_Device.createRenderPass(renderPassInfo);

	// Output the render pass info properties
	std::cout << "Vulkan render pass properties:" << std::endl;
	std::cout << "\tAttachment count: " << renderPassInfo.attachmentCount << std::endl;
	std::cout << "\tSubpass count: " << renderPassInfo.subpassCount << std::endl;
	std::cout << "\tDependency count: " << renderPassInfo.dependencyCount << std::endl;
	std::cout << std::endl;
}

void VulkanInit::InitFramebuffers()
{
	// Create the framebuffers
	m_API.renderPassFrameBuffers.m_Framebuffers.resize(m_API.swapchainInfo.m_ImageCount);

	for (size_t i = 0; i < m_API.swapchainInfo.m_ImageCount; i++)
	{
		vk::ImageView attachments[] = { m_API.swapchainInfo.m_ImageViews[i] };

		vk::FramebufferCreateInfo framebufferInfo;
		framebufferInfo.setRenderPass(m_API.renderPassFrameBuffers.m_RenderPass);
		framebufferInfo.setAttachmentCount(1);
		framebufferInfo.setPAttachments(attachments);
		framebufferInfo.setWidth(m_API.swapchainInfo.m_Extent.width);
		framebufferInfo.setHeight(m_API.swapchainInfo.m_Extent.height);
		framebufferInfo.setLayers(1);

		m_API.renderPassFrameBuffers.m_Framebuffers[i] = m_API.deviceQueue.m_Device.createFramebuffer(framebufferInfo);
	}

	// Output the framebuffer info properties
	std::cout << "Vulkan framebuffer properties:" << std::endl;
	std::cout << "\tCount: " << m_API.renderPassFrameBuffers.m_Framebuffers.size() << std::endl;
	std::cout << std::endl;
}

void VulkanInit::InitCommandPool()
{
	// Create the command pool
	vk::CommandPoolCreateInfo poolInfo;
	poolInfo.setQueueFamilyIndex(m_API.deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::GRAPHICS));

	m_API.commandPool = m_API.deviceQueue.m_Device.createCommandPool(poolInfo);

	// Output the command pool info properties
	std::cout << "Vulkan command pool properties:" << std::endl;
	std::cout << "\tQueue family index: " << poolInfo.queueFamilyIndex << std::endl;
	std::cout << std::endl;
}

void VulkanInit::InitCommandBuffers()
{
	// set m_api.mainCommandBuffer
	m_API.commandBuffers.emplace_back(m_API.deviceQueue.m_Device, m_API.commandPool);
}

void VulkanInit::InitSyncObjects()
{
	vulkan::SemaphoreFence& semaphoreFence = m_API.semaphoreFence;
	
	// Create the semaphores
	vk::SemaphoreCreateInfo semaphoreInfo;

	semaphoreFence.m_ImageAvailable = m_API.deviceQueue.m_Device.createSemaphore(semaphoreInfo);
	semaphoreFence.m_RenderFinished = m_API.deviceQueue.m_Device.createSemaphore(semaphoreInfo);


	// Output the semaphore info properties
	std::cout << "Vulkan semaphore properties:" << std::endl;
	std::cout << "\tImage available: " << m_API.semaphoreFence.m_ImageAvailable << std::endl;
	std::cout << "\tRender finished: " << m_API.semaphoreFence.m_RenderFinished << std::endl;
	std::cout << std::endl;

	// Create the fences
	vk::FenceCreateInfo fenceInfo;
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

	semaphoreFence.m_InFlight = m_API.deviceQueue.m_Device.createFence(fenceInfo);
	semaphoreFence.m_ImagesInFlight.resize(m_API.swapchainInfo.m_ImageCount, vk::Fence());

	// Output the fence info properties
	std::cout << "Vulkan fence properties:" << std::endl;
	std::cout << "\tIn flight: " << m_API.semaphoreFence.m_InFlight << std::endl;
	std::cout << "\tImages in flight: " << m_API.semaphoreFence.m_ImagesInFlight.size() << std::endl;
	std::cout << std::endl;
}
