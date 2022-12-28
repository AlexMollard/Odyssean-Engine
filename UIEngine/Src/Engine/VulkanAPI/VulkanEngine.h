#pragma once

#include "Engine/BaseEngine.h"
#include "VulkanRenderer.h"
#include <vulkan/vulkan.hpp>

struct GLFWwindow;

class VulkanEngine : BaseEngine
{
public:
	VulkanEngine() = default;
	~VulkanEngine();

	void Init(const char* windowName, int width, int height);

	// Returns DT
	float Update();
	void Render();

	bool GetClose() const { return m_close; }

	void* GetWindow();

	const VulkanRenderer* GetRenderer() const { return &m_Renderer; }

private:

	GLFWwindow* window;

	void InitWindow();
	void InitVulkan();
	void Cleanup();
	void CreateInstance();

	bool m_close = true;

	VulkanRenderer m_Renderer;
	vk::Instance m_Instance;
};
