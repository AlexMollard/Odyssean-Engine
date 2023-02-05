#include "pch.h"

#include "Engine.h"

#include "Engine/OpenGlAPI/OpenGLEngine.h"
#include "Engine/VulkanAPI/VulkanEngine.h"

namespace UIEngine
{
void Engine::Init(const char* windowName, int width, int height, GraphicsAPI graphicsAPI)
{
	m_graphicsAPI = graphicsAPI;
	if (graphicsAPI == GraphicsAPI::OpenGL)
	{
		auto engine = _NEW OpenGLEngine;
		engine->Initialize(windowName, width, height);
		m_engine = engine;
		m_close  = false;
		ECS::Instance()->Init();
	}
	else if (graphicsAPI == GraphicsAPI::Vulkan)
	{
		auto engine = _NEW VulkanWrapper::Engine;
		engine->Initialize(windowName, width, height);
		m_engine = engine;
		m_close  = false;
	}
}

Engine::~Engine()
{
	if (m_graphicsAPI == GraphicsAPI::OpenGL)
	{
		auto engine = static_cast<OpenGLEngine*>(m_engine);
		delete engine;
	}
	else if (m_graphicsAPI == GraphicsAPI::Vulkan)
	{
		auto engine = static_cast<VulkanWrapper::Engine*>(m_engine);
		delete engine;
	}
}

float Engine::Update()
{
	if (m_graphicsAPI == GraphicsAPI::OpenGL)
	{
		auto engine = static_cast<OpenGLEngine*>(m_engine);

		m_close = engine->GetClose();

		if (m_close) return -FLT_MAX;

		return engine->Update(m_sceneStateMachine);
	}

	if (m_graphicsAPI == GraphicsAPI::Vulkan)
	{
		auto engine = static_cast<VulkanWrapper::Engine*>(m_engine);

		m_close = engine->GetClose();

		if (m_close) return -FLT_MAX;

		return engine->Update(m_sceneStateMachine);
	}

	return -FLT_MAX;
}

void Engine::Render()
{
	if (m_graphicsAPI == GraphicsAPI::OpenGL)
	{
		auto engine = static_cast<OpenGLEngine*>(m_engine);
		engine->Render(m_sceneStateMachine);
	}
	else if (m_graphicsAPI == GraphicsAPI::Vulkan)
	{
		auto engine = static_cast<VulkanWrapper::Engine*>(m_engine);
		engine->Render(m_sceneStateMachine);
	}
}

SceneStateMachine& Engine::GetSceneStateMachine()
{
	return m_sceneStateMachine;
}

} // namespace UIEngine