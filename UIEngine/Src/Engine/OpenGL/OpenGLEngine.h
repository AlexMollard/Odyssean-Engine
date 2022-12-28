#pragma once

#include "BaseEngine.h"
#include "ImGuiLayer.h"
#include "OpenGl/OpenGLRenderer.h"

class OpenGLEngine : BaseEngine
{
public:
	OpenGLEngine() = default;
	~OpenGLEngine();

	void Init(const char* windowName, int width, int height);

	// Returns DT
	float Update();
	void Render();

	bool GetClose() const { return m_close; }

	void* GetWindow();

	const OpenGLRenderer* GetRenderer() const { return &m_renderer; }

private:
	bool m_close = true;
	OpenGLRenderer m_renderer;
	ImGuiLayer m_ImguiLayer;
};
