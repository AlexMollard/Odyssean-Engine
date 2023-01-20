#include "Container.h"

#include "Mesh.h"
#include <fstream>
#include <iostream>

vk::SwapchainKHR& vulkan::API::Swapchain()
{
	return swapchainInfo.m_Swapchain;
}

vulkan::CommandBuffer& vulkan::API::CreateCommandBuffer()
{
	return commandBuffers.emplace_back(deviceQueue.m_Device, commandPool);
}

vk::ShaderModule vulkan::API::CreateShaderModule(const char* shaderFile)
{
	if (shaderModules.find(shaderFile) != shaderModules.end()) return shaderModules[shaderFile];

	// Read shader code from file
	auto shaderCode = ReadFile(shaderFile);

	// Create shader module
	vk::ShaderModule shaderModule = deviceQueue.m_Device.createShaderModule({ {}, shaderCode.size(), reinterpret_cast<const uint32_t*>(shaderCode.data()) });

	// Store in map
	shaderModules.emplace(shaderFile, shaderModule);

	return shaderModule;
}

void vulkan::API::SetupViewportAndScissor(vk::Viewport& viewport, vk::Rect2D& scissor)
{
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = (float)swapchainInfo.m_Extent.width;
	viewport.height   = (float)swapchainInfo.m_Extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = vk::Offset2D(0, 0);
	scissor.extent = swapchainInfo.m_Extent;
}

void vulkan::API::SetupPipelineLayout(vk::PipelineLayout& pipelineLayout, vk::DescriptorSetLayout& descriptorSetLayout)
{
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.setLayoutCount               = 1;
	pipelineLayoutInfo.pSetLayouts                  = &descriptorSetLayout;
	pipelineLayout                                  = deviceQueue.m_Device.createPipelineLayout(pipelineLayoutInfo);
}

vk::PipelineDepthStencilStateCreateInfo vulkan::API::SetupDepthAndStencilState()
{
	vk::PipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.depthTestEnable                         = VK_TRUE;
	depthStencil.depthWriteEnable                        = VK_TRUE;
	depthStencil.depthCompareOp                          = vk::CompareOp::eLess;
	depthStencil.depthBoundsTestEnable                   = VK_FALSE;
	depthStencil.stencilTestEnable                       = VK_FALSE;
	return depthStencil;
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
}

void vulkan::API::CreateRenderPass()
{
	std::array<vk::AttachmentDescription, 2> attachmentDescriptions;
	
	// Color attachment
	attachmentDescriptions[0] = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
														  swapchainInfo.m_Format,
														  vk::SampleCountFlagBits::e1,
														  vk::AttachmentLoadOp::eClear,
														  vk::AttachmentStoreOp::eStore,
														  vk::AttachmentLoadOp::eDontCare,
														  vk::AttachmentStoreOp::eDontCare,
														  vk::ImageLayout::eUndefined,
														  vk::ImageLayout::ePresentSrcKHR);
	
	// Depth attachment
	attachmentDescriptions[1] = vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
														  vk::Format::eD16Unorm,
														  vk::SampleCountFlagBits::e1,
														  vk::AttachmentLoadOp::eClear,
														  vk::AttachmentStoreOp::eDontCare,
														  vk::AttachmentLoadOp::eDontCare,
														  vk::AttachmentStoreOp::eDontCare,
														  vk::ImageLayout::eUndefined,
														  vk::ImageLayout::eDepthStencilAttachmentOptimal);

	// Attachment references
	vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	// Subpass
	vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorReference, nullptr, &depthReference);

    vk::RenderPass renderPass = deviceQueue.m_Device.createRenderPass(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), attachmentDescriptions, subpass));

	// Create the render pass
	renderPassFrameBuffers.m_RenderPass = renderPass;
}

void vulkan::API::CreateFrameBuffers()
{
	CreateDepthResources();
	
	// Create the framebuffers
	renderPassFrameBuffers.m_Framebuffers.resize(swapchainInfo.m_ImageCount);

	for (size_t i = 0; i < swapchainInfo.m_ImageCount; i++)
	{
		vk::ImageView attachments[] = { swapchainInfo.m_ImageViews[i], depthTexture.imageView };

		vk::FramebufferCreateInfo framebufferInfo;
		framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
		framebufferInfo.setRenderPass(renderPassFrameBuffers.m_RenderPass);
		framebufferInfo.setAttachmentCount(2);
		framebufferInfo.setPAttachments(attachments);
		framebufferInfo.setWidth(swapchainInfo.m_Extent.width);
		framebufferInfo.setHeight(swapchainInfo.m_Extent.height);
		framebufferInfo.setLayers(1);

		// Create the framebuffer
		renderPassFrameBuffers.m_Framebuffers[i] = deviceQueue.m_Device.createFramebuffer(framebufferInfo);
	}
}

void vulkan::API::CreateCommandBuffers()
{
	// Create the command pool
	vk::CommandPoolCreateInfo poolInfo;
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	poolInfo.setQueueFamilyIndex(deviceQueue.GetQueueFamilyIndex(vulkan::QueueType::GRAPHICS));

	commandPool = deviceQueue.m_Device.createCommandPool(poolInfo);

	// This command buffer will be used in the draw frame function
	commandBuffers.emplace_back(deviceQueue.m_Device, commandPool);
	commandBuffers.emplace_back(deviceQueue.m_Device, commandPool);
	commandBuffers.emplace_back(deviceQueue.m_Device, commandPool);
}

void vulkan::API::CreateGraphicsPipeline(const char* vertShader, const char* fragShader, vk::DescriptorSetLayout& descriptorSetLayout)
{
	// Shader stages
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

	vk::ShaderModule vertShaderModule = CreateShaderModule(vertShader);
	vk::ShaderModule fragShaderModule = CreateShaderModule(fragShader);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.stage                             = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module                            = vertShaderModule;
	vertShaderStageInfo.pName                             = "main";
	shaderStages.push_back(vertShaderStageInfo);

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.stage                             = vk::ShaderStageFlagBits::eFragment;
	fragShaderStageInfo.module                            = fragShaderModule;
	fragShaderStageInfo.pName                             = "main";
	shaderStages.push_back(fragShaderStageInfo);

	// Vertex input
	auto bindingDescription    = Vertex::GetBindingDescription();
	auto attributeDescriptions = Vertex::GetVertexAttributes();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexBindingDescriptionCount          = 1;
	vertexInputInfo.pVertexBindingDescriptions             = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount        = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions           = attributeDescriptions.data();

	// Input assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.topology                                 = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable                   = VK_FALSE;

	// Viewport and Scissor
	vk::Viewport viewport;
	vk::Rect2D   scissor;
	SetupViewportAndScissor(viewport, scissor);

	vk::PipelineViewportStateCreateInfo viewportState = {};
	viewportState.viewportCount                       = 1;
	viewportState.pViewports                          = &viewport;
	viewportState.scissorCount                        = 1;
	viewportState.pScissors                           = &scissor;

	// Rasterizer
	vk::PipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.depthClampEnable                         = VK_FALSE;
	rasterizer.rasterizerDiscardEnable                  = VK_FALSE;
	rasterizer.polygonMode                              = vk::PolygonMode::eFill;
	rasterizer.lineWidth                                = 1.0f;
	rasterizer.cullMode                                 = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace                                = vk::FrontFace::eClockwise;
	rasterizer.depthBiasEnable                          = VK_FALSE;
	rasterizer.depthBiasConstantFactor                  = 0.0f; // Optional
	rasterizer.depthBiasClamp                           = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor                     = 0.0f; // Optional

	// Multisampling
	vk::PipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sampleShadingEnable                    = VK_FALSE;
	multisampling.rasterizationSamples                   = vk::SampleCountFlagBits::e1;
	multisampling.minSampleShading                       = 1.0f;     // Optional
	multisampling.pSampleMask                            = nullptr;  // Optional
	multisampling.alphaToCoverageEnable                  = VK_FALSE; // Optional
	multisampling.alphaToOneEnable                       = VK_FALSE; // Optional

	// Depth and stencil testing
	vk::PipelineDepthStencilStateCreateInfo depthStencil = SetupDepthAndStencilState();

	// Color blending
	vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask                        = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable                           = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor                   = vk::BlendFactor::eOne;  // Optional
	colorBlendAttachment.dstColorBlendFactor                   = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.colorBlendOp                          = vk::BlendOp::eAdd;      // Optional
	colorBlendAttachment.srcAlphaBlendFactor                   = vk::BlendFactor::eOne;  // Optional
	colorBlendAttachment.dstAlphaBlendFactor                   = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.alphaBlendOp                          = vk::BlendOp::eAdd;      // Optional

	vk::PipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.logicOpEnable                         = VK_FALSE;
	colorBlending.logicOp                               = vk::LogicOp::eCopy; // Optional
	colorBlending.attachmentCount                       = 1;
	colorBlending.pAttachments                          = &colorBlendAttachment;
	colorBlending.blendConstants[0]                     = 0.0f; // Optional
	colorBlending.blendConstants[1]                     = 0.0f; // Optional
	colorBlending.blendConstants[2]                     = 0.0f; // Optional

	// Dynamic state
	std::vector<vk::DynamicState>      dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };
	vk::PipelineDynamicStateCreateInfo dynamicState  = {};
	dynamicState.dynamicStateCount                   = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates                      = dynamicStates.data();

	// PipelineLayout
	SetupPipelineLayout(graphicsPipelineLayout, descriptorSetLayout); // THIS MIGHT BE BROKY

	// Create graphics pipeline
	vk::GraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.stageCount                     = static_cast<uint32_t>(shaderStages.size());
	pipelineInfo.pStages                        = shaderStages.data();
	pipelineInfo.pVertexInputState              = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState            = &inputAssembly;
	pipelineInfo.pViewportState                 = &viewportState;
	pipelineInfo.pRasterizationState            = &rasterizer;
	pipelineInfo.pMultisampleState              = &multisampling;
	pipelineInfo.pDepthStencilState             = &depthStencil;
	pipelineInfo.pColorBlendState               = &colorBlending;
	pipelineInfo.pDynamicState                  = nullptr; // Optional
	pipelineInfo.layout                         = graphicsPipelineLayout;
	pipelineInfo.renderPass                     = renderPassFrameBuffers.m_RenderPass;
	pipelineInfo.subpass                        = 0;
	pipelineInfo.basePipelineHandle             = nullptr; // Optional
	pipelineInfo.basePipelineIndex              = -1;      // Optional

	vk::ResultValue resValue = deviceQueue.m_Device.createGraphicsPipeline(nullptr, pipelineInfo);

	if (resValue.result != vk::Result::eSuccess) { throw std::runtime_error("Failed to create graphics pipeline!"); }

	graphicsPipeline = resValue.value;
}

std::vector<char> vulkan::API::ReadFile(const char* fileDir)
{
	std::ifstream file(fileDir, std::ios::ate | std::ios::binary);

	if (!file.is_open()) { throw std::runtime_error("Failed to open file!"); }

	size_t            fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void vulkan::API::CreateDepthResources()
{
	// Create the depth image
	vk::ImageCreateInfo imageInfo = {};
	imageInfo.imageType           = vk::ImageType::e2D;
	imageInfo.extent.width        = swapchainInfo.m_Extent.width;
	imageInfo.extent.height       = swapchainInfo.m_Extent.height;
	imageInfo.extent.depth        = 1;
	imageInfo.mipLevels           = 1;
	imageInfo.arrayLayers         = 1;
	imageInfo.format              = vk::Format::eD16Unorm;
	imageInfo.tiling              = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout       = vk::ImageLayout::eUndefined;
	imageInfo.usage               = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	imageInfo.samples             = vk::SampleCountFlagBits::e1;

	depthTexture.image = deviceQueue.m_Device.createImage(imageInfo);

	// Allocate memory for the depth image
	vk::MemoryRequirements memRequirements = deviceQueue.m_Device.getImageMemoryRequirements(depthTexture.image);

	uint32_t memoryTypeIndex;
	deviceQueue.FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, memoryTypeIndex);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize         = memRequirements.size;
	allocInfo.memoryTypeIndex        = memoryTypeIndex;

	depthTexture.memory = deviceQueue.m_Device.allocateMemory(allocInfo);

	// Bind the memory to the depth image
	deviceQueue.m_Device.bindImageMemory(depthTexture.image, depthTexture.memory, 0);

	// Create the depth image view
	vk::ImageViewCreateInfo viewInfo         = {};
	viewInfo.image                           = depthTexture.image;
	viewInfo.viewType                        = vk::ImageViewType::e2D;
	viewInfo.format                          = vk::Format::eD16Unorm;
	viewInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eDepth;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = 1;

	depthTexture.imageView = deviceQueue.m_Device.createImageView(viewInfo);
}

vulkan::API::~API()
{
	// Wait for everything to be free
	deviceQueue.wait();

	syncObjectContainer.cleanup();

	// Destroy pipeline
	deviceQueue.m_Device.destroy(graphicsPipeline);
	deviceQueue.m_Device.destroy(graphicsPipelineLayout);

	// Shader modules
	for (auto shaderModule : shaderModules) { deviceQueue.m_Device.destroyShaderModule(shaderModule.second); }

	// Command buffers
	for (auto commandBuffer : commandBuffers) { deviceQueue.m_Device.freeCommandBuffers(commandPool, commandBuffer.get()); }
	// Command pool
	deviceQueue.m_Device.destroy(commandPool);
	// renderPass
	deviceQueue.m_Device.destroy(renderPassFrameBuffers.m_RenderPass);
	// Framebuffers
	for (auto framebuffer : renderPassFrameBuffers.m_Framebuffers) { deviceQueue.m_Device.destroy(framebuffer); }
	// Depth image
	depthTexture.destroy(deviceQueue.m_Device);

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