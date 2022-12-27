#pragma once

#include "ImGuiLayer.h"
#include "SceneStateMachine.h"
#include "Services.h"
#include "Window.h"

namespace UIEngine
{
	
class Engine
{
public:
	Engine() = default;
	~Engine();

	void Init(const char* windowName, int width, int height);

	// Returns DT
	float Update();
	void Render();

	bool GetClose() const { return m_close; }

	class SceneStateMachine& GetStateMachine() { return m_StateMachine; }

private:

	// Window
	class Window m_Window;

	// Services
	class Services m_Services;

	// Renderer
	class Renderer2D m_Renderer;

	// Scene State Machine
	class SceneStateMachine m_StateMachine;

	// ImGui Layer
	class ImGuiLayer m_ImguiLayer;

	bool m_close = false;
	
	// Audio
	// Audio m_AudioManager;
};
} // namespace UIEngine