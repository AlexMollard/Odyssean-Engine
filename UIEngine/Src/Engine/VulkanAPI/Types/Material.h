#pragma once
#include <string>
#include <vector>

class aiMaterial;
namespace vk
{
struct DescriptorImageInfo;
}

namespace VulkanWrapper
{
class VkContainer;
class SubMesh;

struct Material
{
	std::string name;
	std::string diffuseMap;
	std::string specularMap;
	std::string normalMap;
	std::string heightMap;
	std::string ambientMap;
	std::string emissiveMap;
	std::string shininessMap;
	std::string opacityMap;
	std::string displacementMap;
	std::string lightMap;
	std::string reflectionMap;
	std::string unknownMap;

	bool hasDiffuseMap      = false;
	bool hasSpecularMap     = false;
	bool hasNormalMap       = false;
	bool hasHeightMap       = false;
	bool hasAmbientMap      = false;
	bool hasEmissiveMap     = false;
	bool hasShininessMap    = false;
	bool hasOpacityMap      = false;
	bool hasDisplacementMap = false;
	bool hasLightMap        = false;
	bool hasReflectionMap   = false;
	bool hasUnknownMap      = false;

	int GetTextureCount();

	const std::vector<const vk::DescriptorImageInfo*>& GetDescriptorData(VulkanWrapper::VkContainer& api);

	static void ProcessMaterial(aiMaterial* material, SubMesh& subMesh);

	std::string& operator[](int index);

	std::vector<const vk::DescriptorImageInfo*> descriptorImageInfos;

	const static int MaxCount = 11;
};
} // namespace VulkanWrapper