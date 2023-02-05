#include "pch.h"

#include "VulkanEngine.h"

#include "ImGuiVulkan.h"

namespace VulkanWrapper
{

Engine::~Engine()
{
	ImGuiVulkan::DestroyImgui(m_Init.GetAPI());

	for (auto& texture : m_Init.GetAPI().textureCache)
	{
		texture.second->destroy(m_Init.GetAPI().device);
		delete texture.second;
	}
}

void Engine::Initialize(const char* windowName, int width, int height)
{
	try
	{
		m_Init.Initialize("UIEngine", width, height);

		auto const& impl_ptr = m_Renderer.GetImpl();

		auto vulkan_impl_ptr = dynamic_cast<Renderer::VulkanImpl*>(impl_ptr.get());

		vulkan_impl_ptr->InitRenderer();

		m_Init.GetAPI().SetVulkanRenderer(vulkan_impl_ptr);

		ImGuiVulkan::SetUpImgui(m_Init.GetAPI());
	}
	catch (std::system_error e)
	{
		std::cout << e.what() << std::endl;
	}

	m_Window = m_Init.GetWindow();
}

float Engine::Update(SceneStateMachine& stateMachine)
{
	m_Window->Update();
	m_close = m_Window->GetClose();

	ImGuiVulkan::NewFrame(m_Init.GetAPI());

	stateMachine.Update(m_Window->GetDt());

	ImGuiVulkan::EndFrame(m_Init.GetAPI());
	return 0.0f;
}

void Engine::Render(SceneStateMachine& stateMachine)
{
	// First we check if the window is minimized
	if (m_Window->GetMinimized()) { return; }

	// Render state machine
	stateMachine.Render();

	m_Renderer.Draw();
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