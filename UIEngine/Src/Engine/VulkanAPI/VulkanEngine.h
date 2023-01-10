#pragma once
#include "VulkanInit.h"

#include "Engine/BaseEngine.h"
#include "../ImGuiLayer.h"
#include "BS_thread_pool.hpp"

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

private:
	VulkanInit m_Init;
		
	Window* m_Window;
	bool        m_close      = false;
	bool        m_firstFrame = true;

	// Delta Time
	float m_DT        = 0.0f;
	float m_LastFrame = 0.0f;
};