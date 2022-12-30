#pragma once
#include "VulkanRenderer.h"

#include "Engine/BaseEngine.h"
#include "VulkanInit.h"

struct GLFWwindow;
class VulkanEngine : BaseEngine
{
public:
	VulkanEngine() = default;
	~VulkanEngine();

	void Initialize(const char* windowName, int width, int height) override;

	// Returns DT
	float Update() override;
	void Render() override;

	bool GetClose() const override { return m_close; }
	void* GetWindow() const { return m_Window; }

	const VulkanRenderer* GetRenderer() const { return &m_Renderer; }

private:

	GLFWwindow* m_Window;
	bool m_close = false;

	Init m_Init;
	RenderData m_RenderData;

	VulkanRenderer m_Renderer;
};