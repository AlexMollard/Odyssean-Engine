#include "pch.h"

#include "VulkanEngine.h"

#include "ImGuiVulkan.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
namespace VulkanWrapper
{

Engine::~Engine()
{
	m_Renderer.Destroy();
}

void Engine::Initialize(const char* windowName, int width, int height)
{
	try
	{
		m_Init.Initialize("UIEngine", width, height);
		m_Renderer.Init(&m_Init.GetAPI());
	}
	catch (std::system_error e)
	{
		std::cout << e.what() << std::endl;
	}

	m_Window = m_Init.GetWindow();
}

float Engine::Update()
{
	m_Window->Update();
	m_close = m_Window->GetClose();

	return 0.0f;
}

void Engine::Render()
{
	// First we check if the window is minimized
	if (m_Window->GetMinimized()) { return; }

	// Now we begin all the vulkan commands
	m_Renderer.BeginFrame();

	// We end the frame
	m_Renderer.EndFrame();
}

bool Engine::GetClose() const
{
	return m_close;
}

void* Engine::GetRenderer()
{
	return nullptr;
}
} // namespace VulkanWrapper