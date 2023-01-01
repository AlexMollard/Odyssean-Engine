#include "pch.h"

#include "OpenGLEngine.h"

#include "imgui_impl_opengl3.h"
#include <imgui_impl_glfw.h>

#include "Engine/ECS.h"
#include "Engine/OpenGLAPI/OpenGLWindow.h"
#include "Engine/OpenGLAPI/ShaderOpenGL.h"
#include "Engine/ResourceManager.h"
#include "Engine/SceneStateMachine.h"
#include "Engine/Texture.h"

static OpenGLWindow m_Window;

OpenGLEngine::~OpenGLEngine()
{
	delete pool;
	pool = nullptr;
	ECS::Instance()->Destroy();
}

void OpenGLEngine::Initialize(const char* windowName, int width, int height)
{
	// Create Window
	m_Window.Initialise(width, height, windowName);

	//Audio audioManager = Audio();

	// ImGui Setup
	m_ImguiLayer.Init(m_Window.GetWindow());

	m_close = false;

	pool = new BS::thread_pool(4);
}

float OpenGLEngine::Update(SceneStateMachine& stateMachine)
{
	if (m_Window.Window_shouldClose())
	{
		m_close = true;
		// Return -Max float to exit the game loop
		return -FLT_MAX;
	}

	// Update Window
	m_Window.Update_Window();
	float dt = m_Window.GetDeltaTime();

	m_ImguiLayer.NewFrame(*pool);

	// TODO:
	// Pass in the thread pool to the ecs update so we can multithread the expensive imgui stuff
	auto ECSUpdateFn          = [&]() { ECS::Instance()->Update(*pool); };
	auto StateMachineUpdateFn = [&]() { stateMachine.Update(dt); };

	pool->submit(ECSUpdateFn);
	pool->submit(StateMachineUpdateFn);

	pool->wait_for_tasks();

	ImGui::EndFrame();

	return dt;
}

void OpenGLEngine::Render(SceneStateMachine& stateMachine)
{
	stateMachine.Render(*m_renderer);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_ImguiLayer.UpdateViewPorts();
}

void* OpenGLEngine::GetWindow()
{
	return m_Window.GetWindow();
}