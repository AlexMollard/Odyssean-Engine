#include "pch.h"

#include "VulkanRenderer.h"

#include "ShaderVulkan.h"
#include "VulkanInit.h"

// include assimp
#include "ImGuiVulkan.h"
#include "assimp/scene.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

// All meshes added to this vector will be cleaned up when the program closes
// Aswell as be drawn in the draw frame function
std::vector<vulkan::Mesh> VulkanRenderer::m_Meshes;

const int MAX_FRAMES_IN_FLIGHT = 2;

RenderData VulkanRenderer::SetupRenderData(Init& init)
{
	RenderData data;

	S_ASSERT(CreateSwapchain(init) == 0, "Failed to create swapchain!");

	S_ASSERT(GetQueues(init, data) == 0, "Failed to get the queue families!");

	S_ASSERT(CreateRenderPass(init, data) == 0, "Failed to create the renderpass!");

	S_ASSERT(SetUpPipelineLayout(init, data) == 0, "Failed to setup pipeline layout!");

	S_ASSERT(CreateGraphicsPipeline(init, data) == 0, "Failed to create the graphics pipeline!");

	S_ASSERT(CreateFramebuffers(init, data) == 0, "Failed to create the framebuffers!");

	S_ASSERT(CreateCommandPool(init, data) == 0, "Failed to create the command pool!");

	S_ASSERT(CreateCommandBuffers(init, data) == 0, "Failed to create command buffer!");

	S_ASSERT(CreateSyncObjects(init, data) == 0, "Failed to create the syncObjects!");

	return data;
}

int VulkanRenderer::CreateSwapchain(Init& init)
{
	vk::SurfaceFormatKHR surface_format;
	surface_format.format     = vk::Format::eB8G8R8A8Unorm;
	surface_format.colorSpace = vk::ColorSpaceKHR::eAdobergbLinearEXT;

	vkb::SwapchainBuilder swapchain_builder{ init.device };
	swapchain_builder.set_desired_format(surface_format);
	auto swap_ret = swapchain_builder.set_old_swapchain(init.swapchain).build();
	if (!swap_ret)
	{
		std::cout << swap_ret.error().message() << " " << swap_ret.vk_result() << "\n";
		return -1;
	}
	vkb::destroy_swapchain(init.swapchain);
	init.swapchain = swap_ret.value();

	return 0;
}

int VulkanRenderer::RecreateSwapchain(Init& init, RenderData& data)
{
	init->vkDeviceWaitIdle(init.device);

	init->vkDestroyCommandPool(init.device, data.command_pool, nullptr);

	for (auto framebuffer : data.framebuffers) { init->vkDestroyFramebuffer(init.device, framebuffer, nullptr); }

	init.swapchain.destroy_image_views(data.swapchain_image_views);

	if (0 != CreateSwapchain(init)) return -1;
	if (0 != CreateRenderPass(init, data)) return -1;
	if (0 != SetUpPipelineLayout(init, data)) return -1;
	if (0 != CreateFramebuffers(init, data)) return -1;
	if (0 != CreateCommandPool(init, data)) return -1;
	if (0 != CreateCommandBuffers(init, data)) return -1;
	return 0;
}

int VulkanRenderer::GetQueues(Init& init, RenderData& data)
{
	auto gq = init.device.get_queue(vkb::QueueType::graphics);
	if (!gq.has_value())
	{
		std::cout << "failed to get graphics queue: " << gq.error().message() << "\n";
		return -1;
	}
	data.graphics_queue = gq.value();

	auto pq = init.device.get_queue(vkb::QueueType::present);
	if (!pq.has_value())
	{
		std::cout << "failed to get present queue: " << pq.error().message() << "\n";
		return -1;
	}
	data.present_queue = pq.value();
	return 0;
}

int VulkanRenderer::CreateRenderPass(Init& init, RenderData& data)
{
	vk::Device device = init.device.device;

	vk::AttachmentDescription color_attachment = {};
	color_attachment.format                    = (vk::Format)init.swapchain.image_format;
	color_attachment.samples                   = vk::SampleCountFlagBits::e1;
	color_attachment.loadOp                    = vk::AttachmentLoadOp::eClear;
	color_attachment.storeOp                   = vk::AttachmentStoreOp::eStore;
	color_attachment.stencilLoadOp             = vk::AttachmentLoadOp::eDontCare;
	color_attachment.stencilStoreOp            = vk::AttachmentStoreOp::eDontCare;
	color_attachment.initialLayout             = vk::ImageLayout::eUndefined;
	color_attachment.finalLayout               = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment              = 0;
	color_attachment_ref.layout                  = vk::ImageLayout::eColorAttachmentOptimal;

	vk::SubpassDescription subpass = {};
	subpass.pipelineBindPoint      = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount   = 1;
	subpass.pColorAttachments      = &color_attachment_ref;

	vk::SubpassDependency dependency = {};
	dependency.srcSubpass            = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass            = 0;
	dependency.srcStageMask          = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.srcAccessMask         = {};
	dependency.dstStageMask          = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstAccessMask         = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	vk::RenderPassCreateInfo render_pass_info = {};
	render_pass_info.attachmentCount          = 1;
	render_pass_info.pAttachments             = &color_attachment;
	render_pass_info.subpassCount             = 1;
	render_pass_info.pSubpasses               = &subpass;
	render_pass_info.dependencyCount          = 1;
	render_pass_info.pDependencies            = &dependency;

	auto render_pass = device.createRenderPass(render_pass_info);
	if (!render_pass)
	{
		std::cout << "failed to create render pass\n";
		return -1;
	}

	data.render_pass = render_pass;
	return 0;
}

int VulkanRenderer::SetUpPipelineLayout(Init& init, RenderData& renderData)
{
	vk::Device device = vk::Device{ init.device.device };

	// Create the descriptor set layout (uniform buffer and sampler)
	vk::DescriptorSetLayoutBinding ubo_layout_binding = {};
	ubo_layout_binding.binding                        = 0;
	ubo_layout_binding.descriptorType                 = vk::DescriptorType::eUniformBuffer;
	ubo_layout_binding.descriptorCount                = 1;
	ubo_layout_binding.stageFlags                     = vk::ShaderStageFlagBits::eVertex;
	ubo_layout_binding.pImmutableSamplers             = nullptr; // Optional

	vk::DescriptorSetLayoutBinding sampler_layout_binding = {};
	sampler_layout_binding.binding                        = 1;
	sampler_layout_binding.descriptorCount                = 1;
	sampler_layout_binding.descriptorType                 = vk::DescriptorType::eCombinedImageSampler;
	sampler_layout_binding.pImmutableSamplers             = nullptr;
	sampler_layout_binding.stageFlags                     = vk::ShaderStageFlagBits::eFragment;

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings    = { ubo_layout_binding, sampler_layout_binding };
	vk::DescriptorSetLayoutCreateInfo             layout_info = {};
	layout_info.bindingCount                                  = static_cast<uint32_t>(bindings.size());
	layout_info.pBindings                                     = bindings.data();

	// Create the descriptor set layout
	renderData.descriptor_set_layout = device.createDescriptorSetLayout(layout_info);

	// Create the pipeline layout
	vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.setLayoutCount               = 1;
	pipeline_layout_info.pSetLayouts                  = &renderData.descriptor_set_layout;

	// Create the pipeline layout
	renderData.pipeline_layout = device.createPipelineLayout(pipeline_layout_info);

	// Everything is done
	return 0;
}

int VulkanRenderer::CreateGraphicsPipeline(Init& init, RenderData& data)
{
	vk::Device device = vk::Device{ init.device.device };

	vk::ShaderModule vert_module = ShaderVulkan::CreateShaderModule(init, "../Resources/Shaders/compiled/vulkan_vert.spv");
	vk::ShaderModule frag_module = ShaderVulkan::CreateShaderModule(init, "../Resources/Shaders/compiled/vulkan_frag.spv");
	if (vert_module == NULL || frag_module == NULL) return -1;

	std::array<vk::PipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfos = {
		vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vert_module, "main"),  // Vertex Shader
		vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, frag_module, "main") // Fragment Shader
	};

	// Binding description
	auto binding_description    = vulkan::Mesh::GetBindingDescription();
	auto attribute_descriptions = vulkan::Mesh::GetVertexAttributes();

	// Mesh vertex input (position, color, texcoord)
	vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount          = 1;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions             = &binding_description;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount        = static_cast<uint32_t>(attribute_descriptions.size());
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions           = attribute_descriptions.data();

	// Input assembly (triangle list)
	vk::PipelineInputAssemblyStateCreateInfo input_assembly(vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList);

	vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList);

	vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(vk::PipelineViewportStateCreateFlags(), 1, nullptr, 1, nullptr);

	vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo( //
		vk::PipelineRasterizationStateCreateFlags(),                               // flags
		false,                                                                     // depthClampEnable
		false,                                                                     // rasterizerDiscardEnable
		vk::PolygonMode::eFill,                                                    // polygonMode
		vk::CullModeFlagBits::eBack,                                               // cullMode
		vk::FrontFace::eClockwise,                                                 // frontFace
		false,                                                                     // depthBiasEnable
		0.0f,                                                                      // depthBiasConstantFactor
		0.0f,                                                                      // depthBiasClamp
		0.0f,                                                                      // depthBiasSlopeFactor
		1.0f                                                                       // lineWidth
	);

	vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo( //
		vk::PipelineMultisampleStateCreateFlags(),                             // flags
		vk::SampleCountFlagBits::e1                                            // rasterizationSamples
																			   // other values can be default
	);

	vk::StencilOpState                      stencilOpState(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways);
	vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo( //
		vk::PipelineDepthStencilStateCreateFlags(),                              // flags
		true,                                                                    // depthTestEnable
		true,                                                                    // depthWriteEnable
		vk::CompareOp::eLessOrEqual,                                             // depthCompareOp
		false,                                                                   // depthBoundTestEnable
		false,                                                                   // stencilTestEnable
		stencilOpState,                                                          // front
		stencilOpState                                                           // back
	);

	vk::ColorComponentFlags               colorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState( //
		false,                                                               // blendEnable
		vk::BlendFactor::eZero,                                              // srcColorBlendFactor
		vk::BlendFactor::eZero,                                              // dstColorBlendFactor
		vk::BlendOp::eAdd,                                                   // colorBlendOp
		vk::BlendFactor::eZero,                                              // srcAlphaBlendFactor
		vk::BlendFactor::eZero,                                              // dstAlphaBlendFactor
		vk::BlendOp::eAdd,                                                   // alphaBlendOp
		colorComponentFlags                                                  // colorWriteMask
	);
	vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(vk::PipelineColorBlendStateCreateFlags(), // flags
																			false,                                    // logicOpEnable
																			vk::LogicOp::eNoOp,                       // logicOp
																			pipelineColorBlendAttachmentState,        // attachments
																			{
																				{1.0f, 1.0f, 1.0f, 1.0f}
    } // blendConstants
	);

	std::array<vk::DynamicState, 2>    dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(vk::PipelineDynamicStateCreateFlags(), dynamicStates);

	vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo( //
		vk::PipelineCreateFlags(),                             // flags
		pipelineShaderStageCreateInfos,                        // stages
		&pipelineVertexInputStateCreateInfo,                   // pVertexInputState
		&pipelineInputAssemblyStateCreateInfo,                 // pInputAssemblyState
		nullptr,                                               // pTessellationState
		&pipelineViewportStateCreateInfo,                      // pViewportState
		&pipelineRasterizationStateCreateInfo,                 // pRasterizationState
		&pipelineMultisampleStateCreateInfo,                   // pMultisampleState
		&pipelineDepthStencilStateCreateInfo,                  // pDepthStencilState
		&pipelineColorBlendStateCreateInfo,                    // pColorBlendState
		&pipelineDynamicStateCreateInfo,                       // pDynamicState
		data.pipeline_layout,                                  // layout
		data.render_pass                                       // renderPass
	);

	vk::Result   result;
	vk::Pipeline pipeline;

	std::tie(result, pipeline) = device.createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo);
	switch (result)
	{
	case vk::Result::eSuccess: break;
	case vk::Result::ePipelineCompileRequiredEXT:
		// something meaningfull here
		break;
	default: assert(false); // should never happen
	}

	data.graphics_pipeline = pipeline;
	device.destroyShaderModule(frag_module);
	device.destroyShaderModule(vert_module);

	return 0;
}

int VulkanRenderer::CreateFramebuffers(Init& init, RenderData& data)
{
	data.swapchain_images      = init.swapchain.get_images().value();
	data.swapchain_image_views = init.swapchain.get_image_views().value();

	data.framebuffers.resize(data.swapchain_image_views.size());

	for (size_t i = 0; i < data.swapchain_image_views.size(); i++)
	{
		VkImageView attachments[] = { data.swapchain_image_views[i] };

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass              = data.render_pass;
		framebuffer_info.attachmentCount         = 1;
		framebuffer_info.pAttachments            = attachments;
		framebuffer_info.width                   = init.swapchain.extent.width;
		framebuffer_info.height                  = init.swapchain.extent.height;
		framebuffer_info.layers                  = 1;

		if (init->vkCreateFramebuffer(init.device, &framebuffer_info, nullptr, &data.framebuffers[i]) != VK_SUCCESS)
		{
			return -1; // failed to create framebuffer
		}
	}
	return 0;
}

int VulkanRenderer::CreateCommandPool(Init& init, RenderData& data)
{
	vk::Device device = init.device.device;

	vk::CommandPoolCreateInfo command_pool_create_info;
	command_pool_create_info.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	command_pool_create_info.queueFamilyIndex = init.device.get_queue_index(vkb::QueueType::graphics).value();

	data.command_pool = device.createCommandPool(command_pool_create_info);

	return 0;
}

int VulkanRenderer::CreateCommandBuffers(Init& init, RenderData& data)
{
	vk::Device device = init.device.device;

	data.command_buffers.resize(data.framebuffers.size());

	vk::CommandBufferAllocateInfo alloc_info = {};
	alloc_info.commandPool                   = data.command_pool;
	alloc_info.level                         = vk::CommandBufferLevel::ePrimary;
	alloc_info.commandBufferCount            = (uint32_t)data.command_buffers.size();

	data.command_buffers = device.allocateCommandBuffers(alloc_info);

	return 0;
}

int VulkanRenderer::CreateSyncObjects(Init& init, RenderData& data)
{
	data.available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	data.finished_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
	data.in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
	data.image_in_flight.resize(init.swapchain.image_count, VK_NULL_HANDLE);

	vk::Device device = init.device.device;

	vk::SemaphoreCreateInfo semaphore_info;
	vk::FenceCreateInfo     fence_info;
	fence_info.flags = vk::FenceCreateFlagBits::eSignaled;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		data.available_semaphores[i] = device.createSemaphore(semaphore_info);
		data.finished_semaphore[i]   = device.createSemaphore(semaphore_info);
		data.in_flight_fences[i]     = device.createFence(fence_info);
	}

	return 0;
}

/* Here is the explanation for the code below:
1. We wait for the fence to be signaled, which means that the command buffer has finished executing
2. We acquire the next image to render to from the swapchain
3. We check whether the image is in use, if it is, we wait for the fence to be signaled
4. We mark the image as in use
5. We create the submit information structure
6. We create an array of wait semaphores, which is just the semaphore that we will wait on before executing the command buffer
7. We create an array of wait stages, which is the stage that we will wait on. We will wait on the color attachment output stage, which means that the render pass has finished writing to the image
8. We set the number of wait semaphores, which is just 1
9. We set the wait semaphores, which is the array of wait semaphores
10. We set the wait stages, which is the array of wait stages
11. We set the number of command buffers to submit, which is just 1
12. We set the command buffer, which is the command buffer that we will submit
13. We create an array of signal semaphores, which is just the semaphore that we will signal once the command buffer has finished executing
14. We set the number of signal semaphores, which is just 1
15. We set the signal semaphores, which is the array of signal semaphores
16. We reset the fence
17. We submit the command buffer to the graphics queue
18. We create the present info structure
19. We set the number of wait semaphores, which is just 1
20. We set the wait semaphores, which is the array of wait semaphores
21. We set the number of swapchains, which is just 1
22. We set the swapchains, which is the array of swapchains
23. We set the number of image indices, which is just 1
24. We set the image indices, which is the array of image indices
25. We present the image
26. We update the current frame */

// Where do I add my mesh data to the command buffer?
// Answer: You add it to the command buffer in the DrawFrame function
// The DrawFrame function is called every frame, so you can add your mesh data to the command buffer every frame
// Here is the DrawFrame function:
int VulkanRenderer::DrawFrame(Init& init, RenderData& data)
{
	vk::Device device = init.device.device;

	// Wait for the fence to be signaled.
	device.waitForFences(1, &data.in_flight_fences[data.current_frame], VK_TRUE, UINT64_MAX);

	// Get the index of the next image to be rendered to
	uint32_t image_index = 0;
	// Wait for the next image to be available
	vk::Result result = device.acquireNextImageKHR(init.swapchain.swapchain, UINT64_MAX, data.available_semaphores[data.current_frame], VK_NULL_HANDLE, &image_index);

	// If the swapchain is out of date, we need to recreate it
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		RecreateSwapchain(init, data);
		return 0;
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) { throw std::runtime_error("Failed to acquire swapchain image!"); }

	// Wait for the image to be available
	// If the image is in use, wait for the fence to be signaled
	if (data.image_in_flight[image_index]) { device.waitForFences(1, &data.image_in_flight[image_index], VK_TRUE, UINT64_MAX); }

	// Mark the image as in use
	data.image_in_flight[image_index] = data.in_flight_fences[data.current_frame];

	// Create the submit information structure
	vk::SubmitInfo submit_info;

	// Create an array of wait semaphores, which is just the semaphore that we will wait on before executing the command buffer
	vk::Semaphore wait_semaphores[] = { data.available_semaphores[data.current_frame] };

	// Create an array of wait stages, which is the stage that we will wait on
	// We will wait on the color attachment output stage, which means that the render pass has finished writing to the image
	vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submit_info.waitSemaphoreCount       = 1;
	submit_info.pWaitSemaphores          = wait_semaphores;
	submit_info.pWaitDstStageMask        = wait_stages;
	submit_info.commandBufferCount       = 1;
	submit_info.pCommandBuffers          = &data.command_buffers[image_index];

	// Create an array of signal semaphores, which is just the semaphore that we will signal once the command buffer has finished executing
	vk::Semaphore signal_semaphores[] = { data.finished_semaphore[data.current_frame] };
	submit_info.signalSemaphoreCount  = 1;
	submit_info.pSignalSemaphores     = signal_semaphores;

	// Reset the fence
	device.resetFences(1, &data.in_flight_fences[data.current_frame]);

	// Submit the command buffer to the graphics queue
	data.graphics_queue.submit(1, &submit_info, data.in_flight_fences[data.current_frame]);

	ImGuiVulkan::DrawImgui(init, data);

	// Create the present info structure
	vk::PresentInfoKHR present_info;

	// Create an array of wait semaphores, which is just the semaphore that we will wait on before presenting the image
	vk::Semaphore wait_semaphores2[] = { data.finished_semaphore[data.current_frame] };
	present_info.waitSemaphoreCount  = 1;
	present_info.pWaitSemaphores     = wait_semaphores2;

	// Create an array of swapchains, which is just the swapchain that we will present the image to
	vk::SwapchainKHR swapchains[] = { init.swapchain.swapchain };
	present_info.swapchainCount   = 1;
	present_info.pSwapchains      = swapchains;

	// Create an array of image indices, which is just the index of the image that we will present
	present_info.pImageIndices = &image_index;

	// Present the image
	result = data.present_queue.presentKHR(&present_info);

	// If the swapchain is out of date, we need to recreate it
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) { RecreateSwapchain(init, data); }
	else if (result != vk::Result::eSuccess) { throw std::runtime_error("Failed to present swapchain image!"); }

	// Move to the next frame
	data.current_frame = (data.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

	return 0;
}

int VulkanRenderer::CleanUp(Init& init, RenderData& data)
{
	vk::Device device = init.device.device;

	// Wait for the device to finish
	device.waitIdle();

	// call destroy on all meshes
	for (auto mesh : m_Meshes) { mesh.Destroy(init.device.device); }

	// Destroy the descriptor set layout
	device.destroyDescriptorSetLayout(data.descriptor_set_layout);

	// Destroy the command pool
	device.destroyCommandPool(data.command_pool);

	// Destroy the semaphores
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		device.destroySemaphore(data.available_semaphores[i]);
		device.destroySemaphore(data.finished_semaphore[i]);
		device.destroyFence(data.in_flight_fences[i]);
	}

	// Destroy the swapchain
	vkb::destroy_swapchain(init.swapchain);

	// Destroy pipeline
	device.destroyPipeline(data.graphics_pipeline);

	// Destroy pipeline layout
	device.destroyPipelineLayout(data.pipeline_layout);

	// Destroy the image views
	for (auto image_view : data.swapchain_image_views) { device.destroyImageView(image_view); }

	// Destroy framebuffers
	for (auto framebuffer : data.framebuffers) { device.destroyFramebuffer(framebuffer); }

	// Destroy the render pass
	device.destroyRenderPass(data.render_pass);

	VulkanInit::DestroyVulkan(init);

	return 0;
}

void ProcessMesh(vulkan::Mesh& mesh, aiMesh* ai_mesh, const aiScene* scene)
{
	std::vector<vulkan::Vertex> vertices;
	std::vector<unsigned int>   indices;

	for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
	{
		vulkan::Vertex vertex = {};
		vertex.pos            = glm::vec3(ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z);
		vertex.normal         = glm::vec3(ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z);
		if (ai_mesh->mTextureCoords[0]) { vertex.texCoord = glm::vec2(ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y); }
		else { vertex.texCoord = glm::vec2(0.0f, 0.0f); }
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
	{
		aiFace face = ai_mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) { indices.push_back(face.mIndices[j]); }
	}

	mesh.vertices = vertices;
	mesh.indices  = indices;
}

void ProcessNode(vulkan::Mesh& mesh, aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, ai_mesh, scene);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) { ProcessNode(mesh, node->mChildren[i], scene); }
}

vulkan::Mesh VulkanRenderer::LoadModel(const char* path)
{
	vulkan::Mesh     mesh;
	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return mesh;
	}

	mesh.directory = path;
	mesh.directory = mesh.directory.substr(0, mesh.directory.find_last_of('/'));

	ProcessNode(mesh, scene->mRootNode, scene);

	return mesh;
}

int VulkanRenderer::SetUpMeshBuffers(Init& init, RenderData& renderData, vulkan::Mesh& mesh)
{
	vk::Device device = init.device.device;

	// Vertex buffer
	vk::DeviceSize buffer_size = sizeof(mesh.vertices[0]) * mesh.vertices.size();

	// First create a staging buffer
	vulkan::Buffer staging_buffer = {};

	// Create staging buffer
	VulkanInit::CreateBuffer(init, buffer_size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging_buffer.buffer, staging_buffer.memory);

	// Create vertex buffer
	VulkanInit::CreateBuffer(init, buffer_size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, mesh.vertexBuffer.buffer, mesh.vertexBuffer.memory);

	// Fill the staging buffer
	void* data;
	device.mapMemory(staging_buffer.memory, 0, buffer_size, vk::MemoryMapFlags(), &data);
	memcpy(data, mesh.vertices.data(), (size_t)buffer_size);
	device.unmapMemory(staging_buffer.memory);

	// Copy the data to the staging buffer
	VulkanInit::CopyBuffer(init, renderData, staging_buffer.buffer, mesh.vertexBuffer.buffer, buffer_size);

	// Destroy the staging buffer
	VulkanInit::DestroyBuffer(init, staging_buffer.buffer, staging_buffer.memory);

	// Index buffer
	buffer_size = sizeof(mesh.indices[0]) * mesh.indices.size();

	// Create staging buffer
	VulkanInit::CreateBuffer(init, buffer_size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging_buffer.buffer, staging_buffer.memory);

	// Create index buffer
	VulkanInit::CreateBuffer(init, buffer_size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, mesh.indexBuffer.buffer, mesh.indexBuffer.memory);

	// Fill the staging buffer
	device.mapMemory(staging_buffer.memory, 0, buffer_size, vk::MemoryMapFlags(), &data);
	memcpy(data, mesh.indices.data(), (size_t)buffer_size);
	device.unmapMemory(staging_buffer.memory);

	// Copy the data to the staging buffer
	VulkanInit::CopyBuffer(init, renderData, staging_buffer.buffer, mesh.indexBuffer.buffer, buffer_size);

	// Destroy the staging buffer
	VulkanInit::DestroyBuffer(init, staging_buffer.buffer, staging_buffer.memory);

	// Setup the model matrix buffer
	VulkanInit::CreateBuffer(
		init, sizeof(glm::mat4), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mesh.modelMatrixBuffer.buffer, mesh.modelMatrixBuffer.memory);

	// Now if we have done this correctly we should have a vertex buffer and an index buffer for the mesh
	return 0;
}

int VulkanRenderer::SetUpTexture(Init& init, RenderData& renderData, vulkan::Mesh& mesh)
{
	vk::Device vk_device{ init.device.device };

	// Load the texture
	int            tex_width, tex_height, tex_channels;
	vulkan::Buffer staging_buffer = {};

	void* data = VulkanInit::LoadImage(init, renderData, staging_buffer, mesh, tex_width, tex_height, tex_channels);

	// Create the texture image
	VulkanInit::CreateImage(init,
							tex_width,
							tex_height,
							vk::Format::eR8G8B8A8Unorm,
							vk::ImageTiling::eOptimal,
							vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
							vk::MemoryPropertyFlagBits::eDeviceLocal,
							mesh.texture.image,
							mesh.texture.memory);

	// Transition the image to the correct layout
	VulkanInit::TransitionImageLayout(init, renderData, mesh.texture.image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

	// Copy the data to the texture image
	VulkanInit::CopyBufferToImage(init, renderData, staging_buffer.buffer, mesh.texture.image, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));

	// Transition the image to the correct layout
	VulkanInit::TransitionImageLayout(init, renderData, mesh.texture.image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

	// Destroy the staging buffer
	VulkanInit::DestroyBuffer(init, staging_buffer.buffer, staging_buffer.memory);

	// Create the texture image view
	mesh.texture.imageView = VulkanInit::CreateImageView(init, mesh.texture.image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);

	// Create the texture sampler
	vk::SamplerCreateInfo sampler_info   = {};
	sampler_info.magFilter               = vk::Filter::eLinear;
	sampler_info.minFilter               = vk::Filter::eLinear;
	sampler_info.addressModeU            = vk::SamplerAddressMode::eRepeat;
	sampler_info.addressModeV            = vk::SamplerAddressMode::eRepeat;
	sampler_info.addressModeW            = vk::SamplerAddressMode::eRepeat;
	sampler_info.anisotropyEnable        = VK_FALSE;
	sampler_info.maxAnisotropy           = 16;
	sampler_info.borderColor             = vk::BorderColor::eIntOpaqueBlack;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable           = VK_FALSE;
	sampler_info.compareOp               = vk::CompareOp::eAlways;
	sampler_info.mipmapMode              = vk::SamplerMipmapMode::eLinear;
	sampler_info.mipLodBias              = 0.0f;
	sampler_info.minLod                  = 0.0f;
	sampler_info.maxLod                  = 0.0f;

	if (vk_device.createSampler(&sampler_info, nullptr, &mesh.texture.sampler) != vk::Result::eSuccess)
	{
		std::cout << "Failed to create texture sampler" << std::endl;
		return 1;
	}

	return 0;
}

int VulkanRenderer::SetUpMeshDescriptorInfo(Init& init, vulkan::Mesh& mesh)
{
	// What is a descriptor info?
	// A descriptor info is a structure that contains the information about a descriptor
	// This is used to create a descriptor set, they can be thought of as a uniform buffer object
	// We are going to create a descriptor info for each uniform buffer object we have which in this case is two
	// One for the model matrix and one for a texture

	// Model matrix descriptor info
	mesh.modelMatrixDescriptorInfo.buffer = mesh.modelMatrixBuffer.buffer;
	mesh.modelMatrixDescriptorInfo.offset = 0;
	mesh.modelMatrixDescriptorInfo.range  = sizeof(glm::mat4);

	// Texture descriptor info
	mesh.textureDescriptorInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	mesh.textureDescriptorInfo.imageView   = mesh.texture.imageView;
	mesh.textureDescriptorInfo.sampler     = mesh.texture.sampler;

	return 0;
}

int VulkanRenderer::SetUpMeshDescriptorSets(Init& init, vulkan::Mesh& mesh)
{
	// What is a descriptor set?
	// A descriptor set is a collection of descriptors that are bound to a pipeline (Confusing I know)
	// A descriptor is a buffer or image that is used by a shader, this is what we are going to create well multiple of them
	// A opengl shader would have a uniform buffer object that would be used to pass data to the shader
	// In vulkan we have to create a descriptor set for each uniform buffer object and then bind it to the pipeline
	// This is a bit more complicated but it gives us more control over the data we are passing to the shader

	// Descriptor sets should only be stored in the render data with the mesh having a pointer to the descriptor set
	// This is because the descriptor set is going to be used by the pipeline and the pipeline is going to be stored in the render data

	// Create the descriptor set layout
	// This is the layout of the descriptor set
	// We are going to have a uniform buffer object and a sampler
	// The uniform buffer object will be used to pass the model matrix to the shader
	// The sampler will be used to pass the texture to the shader

	// Create the descriptor set layout
	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {};

	// Binding 0 is the uniform buffer object
	bindings[0].binding         = 0;
	bindings[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags      = vk::ShaderStageFlagBits::eVertex;

	// Binding 1 is the sampler
	bindings[1].binding         = 1;
	bindings[1].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags      = vk::ShaderStageFlagBits::eFragment;

	// Create the descriptor set layout
	vk::DescriptorSetLayoutCreateInfo layout_info = {};
	layout_info.bindingCount                      = static_cast<uint32_t>(bindings.size());
	layout_info.pBindings                         = bindings.data();

	vk::Device vk_device{ init.device.device };

	// Create the descriptor set layout
	mesh.descriptorSetLayout = vk_device.createDescriptorSetLayout(layout_info);

	// So far we have created the descriptor set layout which consists of the bindings
	// being the uniform buffer object and the sampler (mvpmatrix and texture)
	// Now we need to create the descriptor pool
	// The descriptor pool is used to allocate descriptor sets from
	// We need to create a descriptor pool for each mesh
	// We are going to create a descriptor pool with a max of 1 descriptor set

	// Create the descriptor pool
	std::array<vk::DescriptorPoolSize, 2> pool_sizes = {};

	// Uniform buffer object
	pool_sizes[0].type            = vk::DescriptorType::eUniformBuffer;
	pool_sizes[0].descriptorCount = 1;

	// Sampler
	pool_sizes[1].type            = vk::DescriptorType::eCombinedImageSampler;
	pool_sizes[1].descriptorCount = 1;

	// Create the descriptor pool
	vk::DescriptorPoolCreateInfo pool_info = {};
	pool_info.poolSizeCount                = static_cast<uint32_t>(pool_sizes.size());
	pool_info.pPoolSizes                   = pool_sizes.data();
	pool_info.maxSets                      = 1;

	// Create the descriptor pool
	mesh.descriptorPool = vk_device.createDescriptorPool(pool_info);

	// Now we need to allocate the descriptor set from the descriptor pool
	// We need to allocate the descriptor set from the descriptor pool

	// Allocate the descriptor set
	vk::DescriptorSetAllocateInfo alloc_info = {};
	alloc_info.descriptorPool                = mesh.descriptorPool;
	alloc_info.descriptorSetCount            = 1;
	alloc_info.pSetLayouts                   = &mesh.descriptorSetLayout;

	// Allocate the descriptor set
	mesh.descriptorSet = vk_device.allocateDescriptorSets(alloc_info)[0];

	// Now we need to update the descriptor set with the uniform buffer object and the sampler

	// Update the descriptor set
	std::array<vk::WriteDescriptorSet, 2> descriptor_writes = {};

	// Uniform buffer object
	descriptor_writes[0].dstSet          = mesh.descriptorSet;
	descriptor_writes[0].dstBinding      = 0;
	descriptor_writes[0].dstArrayElement = 0;
	descriptor_writes[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptor_writes[0].descriptorCount = 1;
	descriptor_writes[0].pBufferInfo     = &mesh.modelMatrixDescriptorInfo;

	// Sampler
	descriptor_writes[1].dstSet          = mesh.descriptorSet;
	descriptor_writes[1].dstBinding      = 1;
	descriptor_writes[1].dstArrayElement = 0;
	descriptor_writes[1].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
	descriptor_writes[1].descriptorCount = 1;
	descriptor_writes[1].pImageInfo      = &mesh.textureDescriptorInfo;

	// Update the descriptor set
	vk_device.updateDescriptorSets(descriptor_writes, nullptr);

	// Everything is done
	return 0;
}

int VulkanRenderer::RecordCommandBuffers(Init& init, RenderData& renderData, vulkan::Mesh& mesh)
{
	// Record the command buffers
	for (size_t i = 0; i < renderData.command_buffers.size(); i++)
	{
		vk::CommandBuffer  command_buffer  = renderData.command_buffers[i];
		vk::PipelineLayout pipeline_layout = renderData.pipeline_layout;

		// Begin the command buffer
		vk::CommandBufferBeginInfo begin_info = {};
		begin_info.flags                      = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		// Begin the command buffer
		command_buffer.begin(begin_info);

		// Begin the render pass
		vk::RenderPassBeginInfo render_pass_info = {};
		render_pass_info.renderPass              = renderData.render_pass;
		render_pass_info.framebuffer             = renderData.framebuffers[i];
		render_pass_info.renderArea.offset       = vk::Offset2D{ 0, 0 };
		render_pass_info.renderArea.extent       = vk::Extent2D{ init.swapchain.extent.width, init.swapchain.extent.height };

		std::array<vk::ClearValue, 2> clear_values = {};
		clear_values[0].color                      = vk::ClearColorValue(std::array<float, 4>{ 0.1f, 0.1f, 0.2f, 1.0f });
		clear_values[1].depthStencil               = vk::ClearDepthStencilValue(1.0f, 0);

		render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_info.pClearValues    = clear_values.data();

		// Begin the render pass
		command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

		// Bind the pipeline
		command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, renderData.graphics_pipeline);

		vk::Viewport viewport{};
		viewport.x        = 0.0f;
		viewport.y        = 0.0f;
		viewport.width    = (float)init.swapchain.extent.width;
		viewport.height   = (float)init.swapchain.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		command_buffer.setViewport(0, 1, &viewport);

		vk::Rect2D scissor{};
		scissor.offset = vk::Offset2D{ 0, 0 };
		scissor.extent = vk::Extent2D{ init.swapchain.extent.width, init.swapchain.extent.height };
		command_buffer.setScissor(0, 1, &scissor);

		// Bind the vertex buffer and index buffer to the command buffer and draw the mesh
		mesh.AddToCommandBuffer(command_buffer, pipeline_layout);

		// End the render pass
		command_buffer.endRenderPass();

		// End the command buffer
		command_buffer.end();
	}

	// Everything is done
	return 0;
}

int VulkanRenderer::AddMesh(vulkan::Mesh& mesh)
{
	// Add the mesh to the list of meshes
	m_Meshes.push_back(mesh);

	// Return the index of the mesh
	return m_Meshes.size() - 1;
}