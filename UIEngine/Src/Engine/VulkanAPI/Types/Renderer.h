#pragma once
#include "VkContainer.h"
#include "glm/gtc/matrix_transform.hpp"

namespace VulkanWrapper
{
class DescriptorManager;
}

namespace VulkanWrapper
{
class Mesh;
class Renderer
{
public:
	Renderer() = default;
	Renderer(VulkanWrapper::VkContainer* api) : m_API(api){};

	~Renderer();

	void Init(VulkanWrapper::VkContainer* api);
	void Destroy();

	void recreateSwapChain();

	void BeginFrame();
	void EndFrame();

	void RenderUI();

	void UpdateCamera();

private:
	VulkanWrapper::VkContainer*       m_API               = nullptr;
	VulkanWrapper::Mesh*              m_Mesh              = nullptr;
	std::shared_ptr<VulkanWrapper::DescriptorManager> m_DescriptorManager = nullptr;

	glm::mat4   view;
	glm::vec3   m_CameraPos;
	float       m_CameraSpeed;
	glm::vec3   m_CameraUp;
	glm::vec3   m_CameraFront;
	bool        m_FirstMouse;
	double      m_LastX;
	double      m_LastY;
	float       m_Yaw;
	float       m_Pitch;
	GLFWwindow* m_Window;
	float       m_LastFrame;
};
} // namespace VulkanWrapper