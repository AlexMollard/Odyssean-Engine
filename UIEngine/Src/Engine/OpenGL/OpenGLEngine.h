#pragma once

#include "BaseEngine.h"
#include "ImGuiLayer.h"

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

private:
	bool m_close = true;

	ImGuiLayer m_ImguiLayer;
};
