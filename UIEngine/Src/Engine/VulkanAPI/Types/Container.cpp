#include "Container.h"

#include <iostream>

vk::SwapchainKHR& vulkan::API::Swapchain()
{
	return swapchainInfo.m_Swapchain;
}

vulkan::CommandBuffer& vulkan::API::CreateCommandBuffer()
{
	return commandBuffers.emplace_back(deviceQueue.m_Device, commandPool);
}

void vulkan::API::CreateSwapChain()
{
	// Create the swapchain create info
	vk::SwapchainCreateInfoKHR swapchainCreateInfo;
	swapchainCreateInfo.setSurface(window.m_Surface);
	swapchainCreateInfo.setMinImageCount(window.m_SurfaceImageCount);
	swapchainCreateInfo.setImageFormat(window.m_SurfaceFormat.format);
	swapchainCreateInfo.setImageColorSpace(window.m_SurfaceFormat.colorSpace);
	swapchainCreateInfo.setImageExtent(window.m_SurfaceExtent);
	swapchainCreateInfo.setImageArrayLayers(1);
	swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapchainCreateInfo.setPreTransform(window.m_SurfaceCapabilities.currentTransform);
	swapchainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapchainCreateInfo.setPresentMode(window.m_SurfacePresentMode);
	swapchainCreateInfo.setClipped(VK_TRUE);
	swapchainCreateInfo.setOldSwapchain(nullptr);

	// Get the queue family indices
	std::vector<uint32_t> queueFamilyIndices = { deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::GRAPHICS), deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::PRESENT) };

	// Check if the graphics and present queue family indices are the same
	if (queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		swapchainCreateInfo.setQueueFamilyIndexCount(2);
		swapchainCreateInfo.setPQueueFamilyIndices(queueFamilyIndices.data());
	}
	else
	{
		swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
		swapchainCreateInfo.setQueueFamilyIndexCount(0);
		swapchainCreateInfo.setPQueueFamilyIndices(nullptr);
	}

	// Create the swapchain
	swapchainInfo.m_Swapchain = deviceQueue.m_Device.createSwapchainKHR(swapchainCreateInfo);

	// Get the swapchain images
	swapchainInfo.m_Images = deviceQueue.m_Device.getSwapchainImagesKHR(swapchainInfo.m_Swapchain);

	// Create the swapchain image views
	swapchainInfo.m_ImageViews.resize(swapchainInfo.m_Images.size());

	for (size_t i = 0; i < swapchainInfo.m_Images.size(); i++)
	{
		vk::ImageViewCreateInfo imageViewInfo;
		imageViewInfo.setImage(swapchainInfo.m_Images[i]);
		imageViewInfo.setViewType(vk::ImageViewType::e2D);
		imageViewInfo.setFormat(window.m_SurfaceFormat.format);
		imageViewInfo.setComponents({ vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity });
		imageViewInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

		swapchainInfo.m_ImageViews[i] = deviceQueue.m_Device.createImageView(imageViewInfo);
	}

	swapchainInfo.m_Extent       = window.m_SurfaceExtent;
	swapchainInfo.m_Format       = window.m_SurfaceFormat.format;
	swapchainInfo.m_PresentMode  = window.m_SurfacePresentMode;
	swapchainInfo.m_ImageCount   = window.m_SurfaceImageCount;
	swapchainInfo.m_PreTransform = window.m_SurfaceCapabilities.currentTransform;
	swapchainInfo.m_ImageUsage   = vk::ImageUsageFlagBits::eColorAttachment;

	// Output the swapchain info properties
	std::cout << "Vulkan swapchain image properties:" << std::endl;
	std::cout << "  Image count: " << swapchainInfo.m_ImageCount << std::endl;
	std::cout << "  Image format: " << vk::to_string(swapchainInfo.m_Format) << std::endl;
	std::cout << "  Image extent: " << swapchainInfo.m_Extent.width << "x" << swapchainInfo.m_Extent.height << std::endl;
	std::cout << "  Image present mode: " << vk::to_string(swapchainInfo.m_PresentMode) << std::endl;
	std::cout << "  Image usage: " << vk::to_string(swapchainInfo.m_ImageUsage) << std::endl;
	std::cout << "  Image pre-transform: " << vk::to_string(swapchainInfo.m_PreTransform) << std::endl;
	std::cout << std::endl;
}

void vulkan::API::CreateRenderPass()
{
	// Create the render pass
	vk::AttachmentDescription colorAttachment;
	colorAttachment.setFormat(swapchainInfo.m_Format);
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

	renderPassFrameBuffers.m_RenderPass = deviceQueue.m_Device.createRenderPass(renderPassInfo);

	// Output the render pass info properties
	std::cout << "Vulkan render pass properties:" << std::endl;
	std::cout << "\tAttachment count: " << renderPassInfo.attachmentCount << std::endl;
	std::cout << "\tSubpass count: " << renderPassInfo.subpassCount << std::endl;
	std::cout << "\tDependency count: " << renderPassInfo.dependencyCount << std::endl;
	std::cout << std::endl;
}

void vulkan::API::CreateFrameBuffers()
{
	// Create the framebuffers
	renderPassFrameBuffers.m_Framebuffers.resize(swapchainInfo.m_ImageCount);

	for (size_t i = 0; i < swapchainInfo.m_ImageCount; i++)
	{
		vk::ImageView attachments[] = { swapchainInfo.m_ImageViews[i] };

		vk::FramebufferCreateInfo framebufferInfo;
		framebufferInfo.setRenderPass(renderPassFrameBuffers.m_RenderPass);
		framebufferInfo.setAttachmentCount(1);
		framebufferInfo.setPAttachments(attachments);
		framebufferInfo.setWidth(swapchainInfo.m_Extent.width);
		framebufferInfo.setHeight(swapchainInfo.m_Extent.height);
		framebufferInfo.setLayers(1);

		renderPassFrameBuffers.m_Framebuffers[i] = deviceQueue.m_Device.createFramebuffer(framebufferInfo);
	}

	// Output the framebuffer info properties
	std::cout << "Vulkan framebuffer properties:" << std::endl;
	std::cout << "\tCount: " << renderPassFrameBuffers.m_Framebuffers.size() << std::endl;
	std::cout << std::endl;
}

void vulkan::API::CreateCommandBuffers()
{
	// Create the command pool
	vk::CommandPoolCreateInfo poolInfo;
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	poolInfo.setQueueFamilyIndex(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::GRAPHICS));

	commandPool = deviceQueue.m_Device.createCommandPool(poolInfo);

	// Output the command pool info properties
	std::cout << "Vulkan command pool properties:" << std::endl;
	std::cout << "\tQueue family index: " << poolInfo.queueFamilyIndex << std::endl;
	std::cout << std::endl;

	// This command buffer will be used in the draw frame function
	commandBuffers.emplace_back(deviceQueue.m_Device, commandPool);
	commandBuffers.emplace_back(deviceQueue.m_Device, commandPool);
	commandBuffers.emplace_back(deviceQueue.m_Device, commandPool);
}

vulkan::API::~API()
{
	// Wait for everything to be free
	deviceQueue.wait();

	syncObjectContainer.cleanup();

	// Command buffers
	for (auto commandBuffer : commandBuffers) { deviceQueue.m_Device.freeCommandBuffers(commandPool, commandBuffer.get()); }
	// Command pool
	deviceQueue.m_Device.destroy(commandPool);
	// renderPass
	deviceQueue.m_Device.destroy(renderPassFrameBuffers.m_RenderPass);
	// Framebuffers
	for (auto framebuffer : renderPassFrameBuffers.m_Framebuffers) { deviceQueue.m_Device.destroy(framebuffer); }
	// Image views
	for (auto imageView : swapchainInfo.m_ImageViews) { deviceQueue.m_Device.destroy(imageView); }
	// Swapchain
	deviceQueue.m_Device.destroy(swapchainInfo.m_Swapchain);
	// Surface
	instance.destroy(window.m_Surface);
	deviceQueue.m_Device.destroy();
	instance.destroy();
	window.Destroy();
}
