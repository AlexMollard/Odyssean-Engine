#pragma once

#include "BaseEngine.h"
#include "VulkanRenderer.h"

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

	const VulkanRenderer* GetRenderer() const { return &m_renderer; }

private:
	bool m_close = true;

	VulkanRenderer m_renderer;
};
