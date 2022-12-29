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
		auto engine = new OpenGLEngine;
		engine->Init(windowName, width, height);
		m_engine = engine;
		m_close  = false;
	}
	else if (graphicsAPI == GraphicsAPI::Vulkan)
	{
		auto engine = new VulkanEngine;
		engine->Init(windowName, width, height);
		m_engine = engine;
		m_close  = false;
	}
}

Engine::~Engine()
{
	if (m_graphicsAPI == GraphicsAPI::OpenGL)
	{
		OpenGLEngine* engine = static_cast<OpenGLEngine*>(m_engine);
		delete engine;
	}
	else if (m_graphicsAPI == GraphicsAPI::Vulkan)
	{
		VulkanEngine* engine = static_cast<VulkanEngine*>(m_engine);
		delete engine;
	}
}

float Engine::Update()
{
	if (m_graphicsAPI == GraphicsAPI::OpenGL)
	{
		OpenGLEngine* engine = static_cast<OpenGLEngine*>(m_engine);

		m_close = engine->GetClose();

		if (m_close)
			return -FLT_MAX;

		return engine->Update();
	}
	else if (m_graphicsAPI == GraphicsAPI::Vulkan)
	{
		VulkanEngine* engine = static_cast<VulkanEngine*>(m_engine);

		m_close = engine->GetClose();

		if (m_close)
			return -FLT_MAX;

		return engine->Update();
	}

	return -FLT_MAX;
}

void Engine::Render()
{
	if (m_graphicsAPI == GraphicsAPI::OpenGL)
	{
		OpenGLEngine* engine = static_cast<OpenGLEngine*>(m_engine);
		engine->Render();
	}
	else if (m_graphicsAPI == GraphicsAPI::Vulkan)
	{
		VulkanEngine* engine = static_cast<VulkanEngine*>(m_engine);
		engine->Render();
	}
}

const void* Engine::GetRenderer()
{
	if (m_graphicsAPI == GraphicsAPI::OpenGL)
	{
		OpenGLEngine* engine = static_cast<OpenGLEngine*>(m_engine);
		return engine->GetRenderer();
	}
	else if (m_graphicsAPI == GraphicsAPI::Vulkan)
	{
		VulkanEngine* engine = static_cast<VulkanEngine*>(m_engine);
		return engine->GetRenderer();
	}

	return nullptr;
}

} // namespace UIEngine