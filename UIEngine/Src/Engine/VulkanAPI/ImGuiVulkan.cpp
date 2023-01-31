#include "ImGuiVulkan.h"

// Include the ImGui
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "Types/VkContainer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "Engine/ImGuiLayer.h"

int ImGuiVulkan::SetUpImgui(VulkanWrapper::VkContainer& vkContainer)
{
	vk::Device device = vkContainer.device;

	const int SET_COUNT = 100;

	// Create Descriptor Pool
	vk::DescriptorPoolSize poolSizes[] = {
		{			 vk::DescriptorType::eSampler, SET_COUNT},
		{vk::DescriptorType::eCombinedImageSampler, SET_COUNT},
		{        vk::DescriptorType::eSampledImage, SET_COUNT},
		{        vk::DescriptorType::eStorageImage, SET_COUNT},
		{  vk::DescriptorType::eUniformTexelBuffer, SET_COUNT},
		{  vk::DescriptorType::eStorageTexelBuffer, SET_COUNT},
		{       vk::DescriptorType::eUniformBuffer, SET_COUNT},
		{       vk::DescriptorType::eStorageBuffer, SET_COUNT},
		{vk::DescriptorType::eUniformBufferDynamic, SET_COUNT},
		{vk::DescriptorType::eStorageBufferDynamic, SET_COUNT},
		{     vk::DescriptorType::eInputAttachment, SET_COUNT}
	};

	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.poolSizeCount                = static_cast<uint32_t>(std::size(poolSizes));
	poolInfo.pPoolSizes                   = poolSizes;
	poolInfo.maxSets                      = SET_COUNT * static_cast<uint32_t>(std::size(poolSizes));

	vk::DescriptorPool descriptor_pool = device.createDescriptorPool(poolInfo);

	vkContainer.imguiDescriptorPool = descriptor_pool;

	// Set up imgui with docking
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();

		// Setup Dear ImGui docking
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiLayer::SetStyle();


		if (ImGuiVulkan::UsingViewports)
		{
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			style.WindowRounding              = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForVulkan(vkContainer.window.GetWindow(), true);
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance                  = vkContainer.instance;
		initInfo.PhysicalDevice            = vkContainer.deviceQueue.m_PhysicalDevice;
		initInfo.Device                    = vkContainer.device;
		initInfo.Queue                     = vkContainer.deviceQueue.GetQueue(VulkanWrapper::QueueType::GRAPHICS);
		initInfo.QueueFamily               = vkContainer.deviceQueue.GetQueueFamilyIndex(VulkanWrapper::QueueType::GRAPHICS);
		initInfo.PipelineCache             = VK_NULL_HANDLE;
		initInfo.DescriptorPool            = descriptor_pool;
		initInfo.Allocator                 = nullptr;
		initInfo.MinImageCount             = 2;
		initInfo.ImageCount                = vkContainer.swapchainInfo.m_ImageCount;

		// vk result check function
		initInfo.CheckVkResultFn = [](VkResult err) {
			if (err != 0) { throw std::runtime_error("Vulkan error"); }
		};

		// Install handlers
		ImGuiVulkan::SetPlatformIO(vkContainer);

		ImGui_ImplVulkan_Init(&initInfo, vkContainer.renderPassFrameBuffers.m_RenderPass);

		io.Fonts->AddFontFromFileTTF("../Resources/Fonts/FiraCode-Regular.ttf", 16.0f);

		// Upload custom font ../Resources/Fonts/FiraCode-Regular.ttf
		{
			// Use any command queue
			vk::CommandBuffer commandBuffer = vkContainer.deviceQueue.BeginSingleTimeCommands(vkContainer.commandPool);

			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

			vkContainer.deviceQueue.EndSingleTimeCommands(vkContainer.commandPool, vkContainer.deviceQueue.GetQueue(VulkanWrapper::QueueType::GRAPHICS), commandBuffer);

			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}

	return 0;
}

void ImGuiVulkan::DestroyImgui(VulkanWrapper::VkContainer& vkContainer)
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Destroy descriptor pool
	vkDestroyDescriptorPool(vkContainer.device, vkContainer.imguiDescriptorPool, nullptr);
}

void ImGuiVulkan::SetPlatformIO(VulkanWrapper::VkContainer& vkContainer)
{
	auto& platformIO = ImGui::GetPlatformIO();

	// platform create window
	platformIO.Platform_CreateWindow = [](ImGuiViewport* vp) -> void {
		//Using GLFW to create window
		GLFWwindow* window = glfwCreateWindow((int)vp->Size.x, (int)vp->Size.y, "", NULL, NULL);
		vp->PlatformHandle = (void*)window;

		struct ImGui_ImplGlfw_ViewportData
		{
			GLFWwindow* Window;
			bool        WindowOwned;
			int         IgnoreWindowPosEventFrame;
			int         IgnoreWindowSizeEventFrame;
		};

		vp->PlatformUserData = new ImGui_ImplGlfw_ViewportData{ window, true, -1, -1 };

		// Setup GLFW callback
		glfwSetWindowUserPointer(window, vp);
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
			ImGuiViewport* viewport         = (ImGuiViewport*)glfwGetWindowUserPointer(window);
			viewport->PlatformRequestResize = true;
		});
		glfwSetWindowPosCallback(window, [](GLFWwindow* window, int x, int y) {
			ImGuiViewport* viewport       = (ImGuiViewport*)glfwGetWindowUserPointer(window);
			viewport->PlatformRequestMove = true;
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
			ImGuiViewport* viewport        = (ImGuiViewport*)glfwGetWindowUserPointer(window);
			viewport->PlatformRequestClose = true;
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			ImGuiIO& io     = ImGui::GetIO();
			io.MouseDown[0] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != 0;
			io.MouseDown[1] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != 0;
			io.MouseDown[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) != 0;
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
			ImGuiIO& io = ImGui::GetIO();
			io.MouseWheelH += (float)xoffset;
			io.MouseWheel += (float)yoffset;
		});
	};

	// vkCreate function
	platformIO.Platform_CreateVkSurface = [](ImGuiViewport* vp, ImU64 vk_inst, const void* vk_allocators, ImU64* out_vk_surface) -> int {
		// Create GLFW surface
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface((VkInstance)vk_inst, (GLFWwindow*)vp->PlatformHandle, (const VkAllocationCallbacks*)vk_allocators, &surface) != VK_SUCCESS) return 1;

		*out_vk_surface = (ImU64)surface;

		return 0;
	};

	// platform destroy window
	platformIO.Platform_DestroyWindow = [](ImGuiViewport* vp) -> void {
		struct ImGui_ImplGlfw_ViewportData
		{
			GLFWwindow* Window;
			bool        WindowOwned;
			int         IgnoreWindowPosEventFrame;
			int         IgnoreWindowSizeEventFrame;
		};

		auto* data = (ImGui_ImplGlfw_ViewportData*)vp->PlatformUserData;
		if (data && data->WindowOwned) glfwDestroyWindow(data->Window);
		delete data;
		vp->PlatformUserData = vp->PlatformHandle = nullptr;
	};

	// platform show window
	platformIO.Platform_ShowWindow = [](ImGuiViewport* vp) -> void { glfwShowWindow((GLFWwindow*)vp->PlatformHandle); };

	// platform set window pos
	platformIO.Platform_SetWindowPos = [](ImGuiViewport* vp, ImVec2 pos) -> void { glfwSetWindowPos((GLFWwindow*)vp->PlatformHandle, (int)pos.x, (int)pos.y); };

	// platform get window pos
	platformIO.Platform_GetWindowPos = [](ImGuiViewport* vp) -> ImVec2 {
		int x, y;
		glfwGetWindowPos((GLFWwindow*)vp->PlatformHandle, &x, &y);
		return { (float)x, (float)y };
	};

	// platform set window size
	platformIO.Platform_SetWindowSize = [](ImGuiViewport* vp, ImVec2 size) -> void { glfwSetWindowSize((GLFWwindow*)vp->PlatformHandle, (int)size.x, (int)size.y); };

	// platform get window size
	platformIO.Platform_GetWindowSize = [](ImGuiViewport* vp) -> ImVec2 {
		int w, h;
		glfwGetWindowSize((GLFWwindow*)vp->PlatformHandle, &w, &h);
		return { (float)w, (float)h };
	};

	// platform set window focus
	platformIO.Platform_SetWindowFocus = [](ImGuiViewport* vp) -> void { glfwFocusWindow((GLFWwindow*)vp->PlatformHandle); };

	// platform get window focus
	platformIO.Platform_GetWindowFocus = [](ImGuiViewport* vp) -> bool { return glfwGetWindowAttrib((GLFWwindow*)vp->PlatformHandle, GLFW_FOCUSED) != 0; };

	// platform get window dpi scale
	platformIO.Platform_GetWindowDpiScale = [](ImGuiViewport* vp) -> float {
		float xscale, yscale;
		glfwGetWindowContentScale((GLFWwindow*)vp->PlatformHandle, &xscale, &yscale);
		return xscale;
	};

	// platform on window resize
	platformIO.Platform_OnChangedViewport = [](ImGuiViewport* vp) -> void {
		if (vp->PlatformWindowCreated)
		{
			// Update GLFW window size
			if (vp->Flags & ImGuiViewportFlags_NoDecoration)
			{
				// GLFW does not support windowless mode, so we disable decorations when ImGuiViewportFlags_NoDecoration is set.
				// This will trigger a viewport resize, so we disable it to avoid an infinite loop.
				vp->Flags &= ~ImGuiViewportFlags_NoDecoration;
				glfwSetWindowAttrib((GLFWwindow*)vp->PlatformHandle, GLFW_DECORATED, GLFW_FALSE);
				vp->Flags |= ImGuiViewportFlags_NoDecoration;
			}
			else { glfwSetWindowAttrib((GLFWwindow*)vp->PlatformHandle, GLFW_DECORATED, GLFW_TRUE); }

			glfwSetWindowSize((GLFWwindow*)vp->PlatformHandle, (int)vp->Size.x, (int)vp->Size.y);
		}
	};
}

void ImGuiVulkan::NewFrame(VulkanWrapper::VkContainer& vkContainer)
{
	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	// Start the Dear ImGui frame
	ImGui::NewFrame();

	// Docking
	static bool               dockspaceOpen  = true;
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None | ImGuiWindowFlags_NoBackground | ImGuiDockNodeFlags_PassthruCentralNode;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f});

	ImGui::Begin("MainDockSpace", &dockspaceOpen, windowFlags);
	ImGui::PopStyleVar(3);

	// DockSpace
	ImGuiID dockspaceId = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

	// Demo Window
	ImGui::ShowDemoWindow();

	// End Docking
	ImGui::End();
}

void ImGuiVulkan::EndFrame(VulkanWrapper::VkContainer& vkContainer)
{
	ImGui::EndFrame();

	if (ImGuiVulkan::UsingViewports)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();

		ImGui::UpdatePlatformWindows();

		glfwMakeContextCurrent(backup_current_context);
	}
}

void ImGuiVulkan::Render(VulkanWrapper::VkContainer& vkContainer)
{
	ImGui::Render();

	if (ImGuiVulkan::UsingViewports) ImGui::RenderPlatformWindowsDefault();

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vkContainer.commandBuffers[vkContainer.swapchainInfo.getCurrentFrameIndex()].get());
}

void ImGuiVulkan::Resize(VulkanWrapper::VkContainer& vkContainer, int width, int height)
{
	// To be implemented.
}