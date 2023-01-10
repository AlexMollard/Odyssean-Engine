#include "pch.h"

#include "VulkanEngine.h"

#include "ImGuiVulkan.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

VulkanEngine::~VulkanEngine()
{}

void VulkanEngine::Initialize(const char* windowName, int width, int height)
{
	try
	{
		m_Init.Initialize("UIEngine", 1920, 1080);
	}
	catch (std::system_error e)
	{
		std::cout << e.what() << std::endl;
	}
	m_Window = m_Init.GetWindow();
}

float VulkanEngine::Update()
{
	m_Window->Update();
	m_close = m_Window->GetClose();

	return 0.0f;
}

void VulkanEngine::Render()
{}