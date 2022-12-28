#include "pch.h"

#include "OpenGLEngine.h"

#include "Engine/ECS.h"
#include "Engine/OpenGLAPI/OpenGLWindow.h"
#include "Engine/ResourceManager.h"
#include "Engine/SceneStateMachine.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "imgui_impl_opengl3.h"
#include <Tracy.hpp>

static OpenGLWindow m_Window;

OpenGLEngine::~OpenGLEngine()
{
	ECS::Instance()->Destroy();
}

void OpenGLEngine::Init(const char* windowName, int width, int height)
{
	// Create Window
	m_Window.Initialise(width, height, windowName);

	//Audio audioManager = Audio();

	// ImGui Setup
	m_ImguiLayer.Init(m_Window.GetWindow());

	// m_AudioManager.AudioInit();

	m_close = false;
}

float OpenGLEngine::Update()
{
	if (m_Window.Window_shouldClose())
	{
		m_close = true;
		// Return -Max float to exit the game loop
		return -FLT_MAX;
	}

	ZoneScopedN("Frame");
	// Update Window
	m_Window.Update_Window();
	float dt = m_Window.GetDeltaTime();

	// Start the Dear ImGui frame
	m_ImguiLayer.NewFrame();

	//audioManager.system->update();
	//audioManager.Update();

	ECS::Instance()->Update();

	return dt;
}

void OpenGLEngine::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_ImguiLayer.UpdateViewPorts();
	FrameMark;
}

void* OpenGLEngine::GetWindow()
{
	return m_Window.GetWindow();
}
