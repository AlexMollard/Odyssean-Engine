#pragma once
#include "../Types/Mesh.h"
#include "Nodes/Camera.h"
#include "../Types/common.h"
#include "glm/fwd.hpp"
#include <vector>

namespace VulkanWrapper
{
class VkContainer;
struct Mesh;
} // namespace VulkanWrapper

class VkRenderHelper
{
public:
	explicit VkRenderHelper();

	// stop a user from copying a object of this class
	VkRenderHelper(const VkRenderHelper&)            = delete;
	VkRenderHelper& operator=(const VkRenderHelper&) = delete;

	void DrawFrame();

	void BeginFrame();
	void EndFrame();

	void AddMesh(const VulkanWrapper::Mesh& mesh, const glm::mat4& model);

	// Lights
	void AddLight(LIGHT_TYPE type, void* light);
	void ClearLights();

	// Camera
	void SetCamera(node::Camera* camera);

	void ClearMeshes();
private:

	void UpdateDescriptorSets(VulkanWrapper::Mesh& mesh, const glm::mat4& model, const std::vector<std::reference_wrapper<const PointLight>>& pointLights,
	                          const std::vector<std::reference_wrapper<const DirectionalLight>>& directionalLights, const std::vector<std::reference_wrapper<const SpotLight>>& spotLights);
	void RenderMeshes();

	void recreateSwapChain() const;

	// Meshes
	std::vector<std::tuple<VulkanWrapper::Mesh, glm::mat4>> m_Meshes;

	// Lights
	std::vector<std::tuple<LIGHT_TYPE, void*>> m_Lights;
	PointLight m_DefaultPointLight{};
	DirectionalLight m_DefaultDirectionalLight{};
	SpotLight m_DefaultSpotLight{};

	// Camera
	node::Camera* m_Camera = nullptr;
};
