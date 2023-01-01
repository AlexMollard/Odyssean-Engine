#pragma once

#include "Engine/BaseEngine.h"
#include "Engine/ImGuiLayer.h"
#include "Engine/OpenGlAPI/OpenGLRenderer.h"

class OpenGLEngine : BaseEngine
{
public:
	OpenGLEngine() = default;
	~OpenGLEngine();

	void Initialize(const char* windowName, int width, int height);

	// Returns DT
	float Update(SceneStateMachine& stateMachine);
	void  Render(SceneStateMachine& stateMachine);

	bool GetClose() const
	{
		return m_close;
	}

	void* GetWindow();

	const OpenGLRenderer* GetRenderer() const
	{
		return &m_renderer;
	}

	// Cast to BaseEngine
	operator BaseEngine*()
	{
		return static_cast<BaseEngine*>(this);
	}

private:
	bool           m_close = true;
	OpenGLRenderer m_renderer;
	ImGuiLayer     m_ImguiLayer;

	BS::thread_pool* pool = nullptr;
};
