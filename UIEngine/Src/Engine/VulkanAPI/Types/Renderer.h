#pragma once
#include "Container.h"
#include "glm/gtc/matrix_transform.hpp"

namespace vulkan
{
class Mesh;
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

	void UpdateCamera();
private:
	vulkan::API*  m_API       = nullptr;
	vulkan::Mesh* m_Mesh      = nullptr;
	//vulkan::Mesh* m_LightMesh = nullptr;

	glm::mat4 view;
	glm::vec3 m_CameraPos;
	float     m_CameraSpeed;
	glm::vec3 m_CameraUp;
	glm::vec3 m_CameraFront;
	bool      m_FirstMouse;
	double    m_LastX;
	double    m_LastY;
	float     m_Yaw;
	float     m_Pitch;
	GLFWwindow* m_Window;
	float       m_LastFrame;
};
} // namespace vulkan