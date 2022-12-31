#include "ImGuiVulkan.h"

// Include the ImGui
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "VulkanRenderer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "vulkanInit.h"

int ImGuiVulkan::SetUpImgui(Init& init, RenderData& data)
{
	vk::Device device = init.device.device;

	ImguiData imguiData = {};

	// Create Descriptor Pool
	{
		vk::DescriptorPoolSize poolSizes[] = {
			{             vk::DescriptorType::eSampler, 1000},
			{vk::DescriptorType::eCombinedImageSampler, 1000},
			{        vk::DescriptorType::eSampledImage, 1000},
			{        vk::DescriptorType::eStorageImage, 1000},
			{  vk::DescriptorType::eUniformTexelBuffer, 1000},
			{  vk::DescriptorType::eStorageTexelBuffer, 1000},
			{       vk::DescriptorType::eUniformBuffer, 1000},
			{       vk::DescriptorType::eStorageBuffer, 1000},
			{vk::DescriptorType::eUniformBufferDynamic, 1000},
			{vk::DescriptorType::eStorageBufferDynamic, 1000},
			{     vk::DescriptorType::eInputAttachment, 1000}
		};

		vk::DescriptorPoolCreateInfo poolInfo = {};
		poolInfo.poolSizeCount                = static_cast<uint32_t>(std::size(poolSizes));
		poolInfo.pPoolSizes                   = poolSizes;
		poolInfo.maxSets                      = 1000 * static_cast<uint32_t>(std::size(poolSizes));

		imguiData.descriptor_pool = device.createDescriptorPool(poolInfo);
	}

	// Create Descriptor Set Layout
	{
		vk::DescriptorSetLayoutBinding layoutBindings[] = {
			{ 0,              vk::DescriptorType::eSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 2,         vk::DescriptorType::eSampledImage, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 3,         vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 4,   vk::DescriptorType::eUniformTexelBuffer, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 5,   vk::DescriptorType::eStorageTexelBuffer, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 6,        vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 7,        vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 8, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{ 9, vk::DescriptorType::eStorageBufferDynamic, 1, vk::ShaderStageFlagBits::eFragment, nullptr},
			{10,      vk::DescriptorType::eInputAttachment, 1, vk::ShaderStageFlagBits::eFragment, nullptr}
		};

		vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.bindingCount                      = static_cast<uint32_t>(std::size(layoutBindings));
		layoutInfo.pBindings                         = layoutBindings;

		imguiData.descriptor_set_layout = device.createDescriptorSetLayout(layoutInfo);
	}

	// Create Descriptor Set
	{
		vk::DescriptorSetAllocateInfo allocInfo = {};
		allocInfo.descriptorPool                = imguiData.descriptor_pool;
		allocInfo.descriptorSetCount            = 1;
		allocInfo.pSetLayouts                   = &imguiData.descriptor_set_layout;

		imguiData.descriptor_set = device.allocateDescriptorSets(allocInfo)[0];
	}

	// Create the command pool
	{
		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.queueFamilyIndex          = init.device.get_queue_index(vkb::QueueType::graphics).value();
		poolInfo.flags                     = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		imguiData.command_pool = device.createCommandPool(poolInfo);
	}

	// Create the command buffers
	{
		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool                   = data.command_pool;
		allocInfo.level                         = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount            = data.command_buffers.size();

		imguiData.command_buffers = device.allocateCommandBuffers(allocInfo);
	}

	// Create the Render Pass
	{
		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.format                    = (vk::Format)init.swapchain.image_format;
		colorAttachment.samples                   = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp                    = vk::AttachmentLoadOp::eLoad;
		colorAttachment.storeOp                   = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp             = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp            = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout             = vk::ImageLayout::eColorAttachmentOptimal;
		colorAttachment.finalLayout               = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment              = 0;
		colorAttachmentRef.layout                  = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.pipelineBindPoint      = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount   = 1;
		subpass.pColorAttachments      = &colorAttachmentRef;

		vk::RenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.attachmentCount          = 1;
		renderPassInfo.pAttachments             = &colorAttachment;
		renderPassInfo.subpassCount             = 1;
		renderPassInfo.pSubpasses               = &subpass;

		imguiData.render_pass = device.createRenderPass(renderPassInfo);
	}

	// Set up imgui
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForVulkan(init.window, true);
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance                  = init.instance.instance;
		initInfo.PhysicalDevice            = init.device.physical_device;
		initInfo.Device                    = device;
		initInfo.Queue                     = init.device.get_queue(vkb::QueueType::graphics).value();
		initInfo.QueueFamily               = init.device.get_queue_index(vkb::QueueType::graphics).value();
		initInfo.PipelineCache             = VK_NULL_HANDLE;
		initInfo.DescriptorPool            = imguiData.descriptor_pool;
		initInfo.Allocator                 = nullptr;
		initInfo.MinImageCount             = 2;
		initInfo.ImageCount                = init.swapchain.image_count;
		initInfo.CheckVkResultFn           = nullptr;
		ImGui_ImplVulkan_Init(&initInfo, data.render_pass);

		// Upload Fonts
		{
			vk::CommandBuffer commandBuffer = VulkanInit::BeginSingleTimeCommands(init, data);

			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

			VulkanInit::EndSingleTimeCommands(init, data, commandBuffer);

			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}

	data.imgui_data = imguiData;
	return 0;
}

int ImGuiVulkan::UpdateImgui(Init& init, RenderData& data)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::EndFrame();

	return 0;
}

int ImGuiVulkan::RecordImguiCommandBuffers(Init& init, RenderData& data)
{
	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags                      = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

	for (size_t i = 0; i < data.command_buffers.size(); i++)
	{
		vk::CommandBuffer commandBuffer = data.imgui_data.command_buffers[i];

		commandBuffer.begin(beginInfo);

		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass              = data.imgui_data.render_pass;
		renderPassInfo.framebuffer             = data.framebuffers[i];
		renderPassInfo.renderArea.offset       = vk::Offset2D(0, 0);
		renderPassInfo.renderArea.extent       = init.swapchain.extent;

		vk::ClearValue clearColor      = vk::ClearColorValue(std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f });
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues    = &clearColor;

		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

		commandBuffer.endRenderPass();
		commandBuffer.end();
	}

	return 0;
}

int ImGuiVulkan::DrawImgui(Init& init, RenderData& data)
{
	vk::SubmitInfo submitInfo     = {};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &data.imgui_data.command_buffers[data.current_frame];

	vk::Result result = data.graphics_queue.submit(1, &submitInfo, data.in_flight_fences[data.current_frame]);

	if (result != vk::Result::eSuccess)
	{
		std::cout << "Failed to submit draw command buffer!" << std::endl;
		return 1;
	}

	return 0;
}
