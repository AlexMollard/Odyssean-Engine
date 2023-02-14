#pragma once
#include "../Types/SubMesh.h"
#include <string>
#include <vector>

namespace VulkanWrapper
{
struct Mesh;
struct DeviceQueue;
} // namespace VulkanWrapper

struct aiScene;
struct aiMesh;
struct aiNode;
namespace VulkanWrapper
{
struct VkContainer;
}

namespace vk
{
struct Device;
}

class MeshHelper
{
public:
	// Helper function to load a model from a file
	static std::vector<VulkanWrapper::SubMesh> LoadModel(const std::string& filePath);

	// Helper function to create a mesh from a set of submeshes
	static void CreateMesh(VulkanWrapper::VkContainer& api, const std::vector<VulkanWrapper::SubMesh>& subMeshes, VulkanWrapper::Mesh& mesh);

	// Helper function to destroy a mesh
	static void DestroyMesh(VulkanWrapper::Mesh& mesh);

private:
	// Helper function to process a mesh in a scene
	static bool ProcessMesh(const aiMesh* mesh, VulkanWrapper::SubMesh& subMesh);

	static glm::vec2 ComputeTexCoords(glm::vec3 pos);
};
