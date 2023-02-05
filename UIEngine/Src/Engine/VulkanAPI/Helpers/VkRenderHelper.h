#pragma once
#include "glm/fwd.hpp"
#include <vector>
#include "../Types/Mesh.h"
#include "Nodes/Camera.h"

namespace VulkanWrapper
{
struct VkContainer;
struct Mesh;
}

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

	void SetCamera(node::Camera* camera);

private:
	void clearMeshes();

	void UpdateDescriptorSets(VulkanWrapper::Mesh& mesh, const glm::mat4& model);
	void RenderMeshes();

	void recreateSwapChain();

	std::vector<std::tuple<VulkanWrapper::Mesh, glm::mat4>> m_Meshes;

	node::Camera* m_Camera = nullptr;
};
