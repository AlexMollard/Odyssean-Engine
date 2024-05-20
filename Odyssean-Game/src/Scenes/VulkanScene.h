#pragma once
#include "Engine/VulkanAPI/Types/VkContainer.h"
#include "glm/glm.hpp"
#include <string_view>

#include "Engine/Scene.h"

namespace VulkanWrapper
{
struct Mesh;
class VkContainer;
class DescriptorManager;
} // namespace VulkanWrapper

class VulkanScene : public Scene
{
public:
	explicit VulkanScene(std::string_view inName) : name(inName){};

	void OnCreate() override{ /*To Be Impelmented*/ };
	void Enter() override;

	void Exit() override;
	void OnDestroy() override{ /*To Be Impelmented*/ };

	void Update(float deltaTime) override;
	void Draw() override;

private:
	VulkanWrapper::VkContainer& m_API = VulkanWrapper::VkContainer::instance();

	Renderer::VulkanImpl& m_Renderer = *m_API.renderer;

	std::shared_ptr<VulkanWrapper::DescriptorManager> m_DescriptorManager = nullptr;

	std::shared_ptr<VulkanWrapper::Mesh> m_KnotMesh = nullptr;
	std::shared_ptr<VulkanWrapper::Mesh> m_SceneMesh = nullptr;
	std::shared_ptr<VulkanWrapper::Mesh> m_CubeMesh = nullptr;

	node::Camera m_Camera;

	std::string name;

	std::pair<float, float> m_prevMousePos = { 0.0f, 0.0f };

	PointLight m_PointLight{};
};