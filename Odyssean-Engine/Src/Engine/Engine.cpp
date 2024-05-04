#include "pch.h"

#include "Engine.h"

#include "InputManager.h"

#include "Engine/OpenGlAPI/OpenGLEngine.h"
#include "Engine/VulkanAPI/VulkanEngine.h"

namespace UIEngine
{
void Engine::Init(const char* windowName, int width, int height, GraphicsAPI graphicsAPI)
{
	m_graphicsAPI = graphicsAPI;
	if (graphicsAPI == GraphicsAPI::OpenGL)
	{
		auto engine = new OpenGLEngine;
		engine->Initialize(windowName, width, height);
		m_engine = engine;
		m_close  = false;
		ECS::Instance()->Init();
	}
	else if (graphicsAPI == GraphicsAPI::Vulkan)
	{
		auto engine = new VulkanWrapper::Engine;
		engine->Initialize(windowName, width, height);
		m_engine = engine;
		m_close  = false;
		InputManager::GetInstance().Init(VulkanWrapper::VkContainer::instance().window.GetWindow());
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
	InputManager::GetInstance().Update();

	auto close = [this]<typename T>(T* engine)
	{
		if (engine->GetClose())
			return -FLT_MAX;
		return engine->Update(m_sceneStateMachine);
	};

	if (m_graphicsAPI == GraphicsAPI::OpenGL)
	{
		auto engine = static_cast<OpenGLEngine*>(m_engine);
		m_close     = close(engine);
		return m_close;
	}

	if (m_graphicsAPI == GraphicsAPI::Vulkan)
	{
		auto engine = static_cast<VulkanWrapper::Engine*>(m_engine);
		m_close     = close(engine);
		return m_close;
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