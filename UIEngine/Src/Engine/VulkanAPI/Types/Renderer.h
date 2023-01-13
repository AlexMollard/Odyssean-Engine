#pragma once
#include "Container.h"

namespace vulkan
{
class Renderer
{
public:
	Renderer() = default;
	Renderer(vulkan::API* api) : m_API(api){};

	~Renderer();

	void Init(vulkan::API* api);
	void Destroy();

	void recreateSwapChain();

	void BeginFrame();
	void EndFrame();

	void RenderUI();

private:
	vulkan::API* m_API = nullptr;
};
} // namespace vulkan