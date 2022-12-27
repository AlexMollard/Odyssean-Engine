#include "pch.h"

#include "Engine.h"

#include "ECS.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <Tracy.hpp>

namespace UIEngine
{

void Engine::Init(const char* windowName, int width, int height)
{
	// Create Window
	m_Window.Initialise(width, height, windowName);

	auto shaderManager = ResourceManager<Shader>();
	shaderManager.Initialise("Shader Manager");

	auto textureManager = ResourceManager<Texture>();
	textureManager.Initialise("Texture Manager");

	// Load shaders
	Shader* basicShader = shaderManager.Load("../Resources/Shaders/lit.vert", "../Resources/Shaders/lit.frag").get();
	Shader* fontShader  = shaderManager.Load("../Resources/Shaders/font.vert", "../Resources/Shaders/font.frag").get();

	m_Renderer.Init(nullptr, basicShader, fontShader);
	ECS::Instance()->Init(&m_Renderer);

	//Audio audioManager = Audio();

	m_Services.SetWindow(&m_Window);
	m_Services.SetShaderManager(&shaderManager);
	m_Services.SetTextureManager(&textureManager);

	m_StateMachine.Init(&m_Services);

	// ImGui Setup
	m_ImguiLayer.Init(m_Window.GetWindow());

	// m_AudioManager.AudioInit();
}

Engine::~Engine()
{
	ECS::Instance()->Destroy();
}

// Lambda function for updating the engine
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

	m_StateMachine.update(dt);

	//audioManager.system->update();
	//audioManager.Update();

	ECS::Instance()->Update();

	return dt;
}

// Lambda function for rendering the engine
void Engine::Render()
{
	m_StateMachine.render(m_Window);
	ImGui::Render();
	m_Renderer.Draw();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_ImguiLayer.UpdateViewPorts();
	FrameMark;
}
} // namespace UIEngine