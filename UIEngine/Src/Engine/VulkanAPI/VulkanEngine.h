#pragma once
#include "../ImGuiLayer.h"
#include "Types/Renderer.h"
#include "VulkanInit.h"

#include "Engine/BaseEngine.h"

namespace vulkan
{
class Engine : BaseEngine
{
public:
	Engine() = default;
	~Engine();

	void Initialize(const char* windowName, int width, int height) override;

	// Returns DT
	float Update() override;
	void  Render() override;

	bool GetClose() const override;

	void* GetRenderer();

private:
	VulkanInit       m_Init;
	vulkan::Renderer m_Renderer;

	Window* m_Window;
	bool    m_close      = false;
	bool    m_firstFrame = true;

	// Delta Time
	float m_DT        = 0.0f;
	float m_LastFrame = 0.0f;
};
} // namespace vulkan