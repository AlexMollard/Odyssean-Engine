#pragma once
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace VulkanWrapper
{
struct VkContainer;
}

class ImGuiVulkan
{
public:
	static int SetUpImgui(VulkanWrapper::VkContainer& vkContainer);
	static void DestroyImgui(VulkanWrapper::VkContainer& vkContainer);

	static void SetPlatformIO(VulkanWrapper::VkContainer& vkContainer);

	static void NewFrame(VulkanWrapper::VkContainer& vkContainer);
	static void EndFrame(VulkanWrapper::VkContainer& vkContainer);
	static void Render(VulkanWrapper::VkContainer& vkContainer);

	static void Resize(VulkanWrapper::VkContainer& vkContainer, int width, int height);

	static const bool UsingViewports = false;
};
