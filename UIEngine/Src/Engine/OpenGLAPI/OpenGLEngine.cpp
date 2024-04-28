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

OpenGLEngine::~OpenGLEngine()
{
	delete pool;
	pool = nullptr;
	ECS::Instance()->Destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete FBO
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteTextures(1, &m_Texture);
}

void OpenGLEngine::Initialize(const char* windowName, int width, int height)
{
	static OpenGLWindow& m_Window = OpenGLWindow::Instance();

	// Create Window
	m_Window.Initialise(width, height, windowName);

	// Audio audioManager = Audio();

	// ImGui Setup
	m_ImguiLayer.Init(m_Window.GetWindow());
	ImGui_ImplOpenGL3_Init("#version 450");

	m_close = false;

	pool = new BS::thread_pool(4);

	// Create FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);
}

float OpenGLEngine::Update(SceneStateMachine& stateMachine)
{
	static OpenGLWindow& m_Window = OpenGLWindow::Instance();

	if (m_Window.Window_shouldClose())
	{
		m_close = true;
		// Return -Max float to exit the game loop
		return -FLT_MAX;
	}

	// Update Window
	m_Window.Update_Window();
	float dt = m_Window.GetDeltaTime();

	ECS::Instance()->Update(*pool);

	ImGui_ImplOpenGL3_NewFrame();
	m_ImguiLayer.NewFrame(*pool);

	auto StateMachineUpdateFn = [&]() { stateMachine.Update(dt); };

	pool->submit_task(StateMachineUpdateFn).wait();

	return dt;
}

void OpenGLEngine::Render(SceneStateMachine& stateMachine)
{
	// Bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Clear fbo
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render state machine
	stateMachine.Render();

	// Unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Render fbo to imgui
	// Render the fbo to imgui
	ImGui::Begin("Render Texture");
	int tabWidth  = static_cast<int>(ImGui::GetContentRegionAvail().x);
	int tabHeight = static_cast<int>(ImGui::GetContentRegionAvail().y);
	ImGui::Image((void*)(intptr_t)m_fbo, ImVec2(tabWidth, tabHeight));
	ImGui::End();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_ImguiLayer.UpdateViewPorts();
}

void* OpenGLEngine::GetWindow()
{
	return OpenGLWindow::Instance().GetWindow();
}