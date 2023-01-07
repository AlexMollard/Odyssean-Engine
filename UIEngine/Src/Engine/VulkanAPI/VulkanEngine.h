#pragma once
#include "VulkanInit.h"
#include "VulkanRenderer.h"

#include "Engine/BaseEngine.h"
#include "../ImGuiLayer.h"
#include "BS_thread_pool.hpp"

struct GLFWwindow;
class VulkanEngine : BaseEngine
{
public:
	VulkanEngine() = default;
	~VulkanEngine();

	void Initialize(const char* windowName, int width, int height) override;

	// Returns DT
	float Update() override;
	void  Render() override;

	bool GetClose() const override
	{
		return m_close;
	}
	void* GetWindow() const
	{
		return m_Window;
	}

	const VulkanRenderer* GetRenderer() const
	{
		return &m_Renderer;
	}

private:

	ImGuiLayer  m_ImguiLayer;
	BS::thread_pool* pool = nullptr;
		
	GLFWwindow* m_Window;
	bool        m_close      = false;
	bool        m_firstFrame = true;

	Init       m_Init;
	RenderData m_RenderData;

	vulkan::Mesh m_Mesh;

	VulkanRenderer m_Renderer;

	// Delta Time
	float m_DT        = 0.0f;
	float m_LastFrame = 0.0f;
};