#include "pch.h"

#include "Engine.h"

#include "ECS.h"
#include "ImGuiLayer.h"
#include "SceneStateMachine.h"
#include "Services.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <Tracy.hpp>

static Window m_Window;
static ImGuiLayer m_ImguiLayer;
static Renderer2D m_Renderer;
static Services m_Services;
static ResourceManager<Shader> m_shaderManager;
static ResourceManager<Texture> m_TextureManager;

namespace UIEngine
{

void Engine::Init(const char* windowName, int width, int height)
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

	m_Renderer.Init(nullptr, basicShader, fontShader);
	ECS::Instance()->Init(&m_Renderer);

	//Audio audioManager = Audio();

	m_Services.SetWindow(&m_Window);
	m_Services.SetShaderManager(&m_shaderManager);
	m_Services.SetTextureManager(&m_TextureManager);

	// ImGui Setup
	m_ImguiLayer.Init(m_Window.GetWindow());

	// m_AudioManager.AudioInit();
}

Engine::~Engine()
{
	ECS::Instance()->Destroy();
}

float Engine::Update()
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

void Engine::Render()
{
	ImGui::Render();
	m_Renderer.Draw();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_ImguiLayer.UpdateViewPorts();
	FrameMark;
}

void* Engine::GetWindow() 
{
	return m_Window.GetWindow();
}

} // namespace UIEngine