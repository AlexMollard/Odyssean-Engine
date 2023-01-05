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

	// Create Descriptor Pool
	vk::DescriptorPoolSize poolSizes[] = {
		{			 vk::DescriptorType::eSampler, 1000},
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

	vk::DescriptorPool descriptor_pool = device.createDescriptorPool(poolInfo);

	data.imgui_descriptor_pool = descriptor_pool;

	// Set up imgui with docking
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForVulkan(init.window, true);
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance                  = init.instance.instance;
		initInfo.PhysicalDevice            = init.device.physical_device;
		initInfo.Device                    = init.device;
		initInfo.Queue                     = init.device.get_queue(vkb::QueueType::graphics).value();
		initInfo.QueueFamily               = init.device.get_queue_index(vkb::QueueType::graphics).value();
		initInfo.PipelineCache             = VK_NULL_HANDLE;
		initInfo.DescriptorPool            = descriptor_pool;
		initInfo.Allocator                 = nullptr;
		initInfo.MinImageCount             = 2;
		initInfo.ImageCount                = init.swapchain.image_count;

		// vk result check function
		initInfo.CheckVkResultFn           = [](VkResult err) {
			if (err != 0)
			{
				throw std::runtime_error("Vulkan error");
			}
		};

		// Install the create window handler

		ImGui_ImplVulkan_Init(&initInfo, data.render_pass);

		// Upload Fonts
		{
			vk::CommandBuffer commandBuffer = VulkanInit::BeginSingleTimeCommands(init, data);

			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

			VulkanInit::EndSingleTimeCommands(init, data, commandBuffer);

			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}		

		// Setup Dear ImGui docking
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	return 0;
}

int ImGuiVulkan::UpdateImgui(Init& init, RenderData& data)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	// Make the entire window be a dockspace
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::ShowDemoWindow();
	ImGui::ShowAboutWindow();
	ImGui::ShowDebugLogWindow();
	
	return 0;
}

void ImGuiVulkan::DestroyImgui(Init& init, RenderData& data)
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Destroy descriptor pool
	vkDestroyDescriptorPool(init.device.device, data.imgui_descriptor_pool, nullptr);
}