#include "pch.h"

#include "OpenGLEngine.h"

#include "ECS.h"
#include "OpenGL/OpenGLWindow.h"
#include "SceneStateMachine.h"
#include <Tracy.hpp>
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"
#include "imgui_impl_opengl3.h"


static OpenGLWindow m_Window;
static ResourceManager<Shader> m_shaderManager;
static ResourceManager<Texture> m_TextureManager;

OpenGLEngine::~OpenGLEngine()
{
	ECS::Instance()->Destroy();
}

void OpenGLEngine::Init(const char* windowName, int width, int height)
{
	// Create Window
	m_Window.Initialise(width, height, windowName);

	m_shaderManager = ResourceManager<Shader>();
	m_shaderManager.Initialise("Shader Manager");

	m_TextureManager = ResourceManager<Texture>();
	m_TextureManager.Initialise("Texture Manager");

	// Load shaders
	Shader* basicShader = m_shaderManager.Load("../Resources/Shaders/lit.vert", "../Resources/Shaders/lit.frag").get();
	Shader* fontShader  = m_shaderManager.Load("../Resources/Shaders/font.vert", "../Resources/Shaders/font.frag").get();

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
