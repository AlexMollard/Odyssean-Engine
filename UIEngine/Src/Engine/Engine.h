#pragma once
#include "Engine/BaseEngine.h"
#include "Engine/BaseRenderer.h"

// Graphics API enum
enum class GraphicsAPI
{
	OpenGL,
	Vulkan
};

namespace UIEngine
{
class Engine
{
public:
	Engine() = default;
	~Engine();

	void Init(const char* windowName, int width, int height, GraphicsAPI graphicsAPI = GraphicsAPI::OpenGL);

	// Returns DT
	float Update();
	void Render();

	bool GetClose() const
	{
		return m_close;
	}

	SceneStateMachine& GetSceneStateMachine();

private:
	bool m_close = true;

	// Graphics API
	GraphicsAPI m_graphicsAPI = GraphicsAPI::OpenGL;

	void* m_engine = nullptr;

	SceneStateMachine m_sceneStateMachine;
};
} // namespace UIEngine