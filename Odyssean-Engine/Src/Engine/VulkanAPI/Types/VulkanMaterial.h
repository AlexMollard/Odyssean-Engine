#pragma once
#include <map>
#include <string>
#include <vector>

namespace vk
{
struct DescriptorImageInfo;
}

class aiMaterial;

namespace VulkanWrapper
{
class VkContainer;
class SubMesh;

struct VulkanMaterial
{
	std::string m_Name;

	std::map<int, std::string> m_Textures;

	int GetTextureCount();

	const std::vector<const vk::DescriptorImageInfo*>& GetDescriptorData(VulkanWrapper::VkContainer& api);

	static void ProcessMaterial(aiMaterial* material, SubMesh& subMesh, VulkanWrapper::VkContainer& api);

	std::string& operator[](int index);

	std::vector<const vk::DescriptorImageInfo*> m_DescriptorImageInfos;

	bool hasTexture(int type);

	static VulkanMaterial CreateDebugMaterial(VulkanWrapper::VkContainer& api);

	// Returns a bitfield of missing textures
	uint16_t GetMissingTextures();
};
} // namespace VulkanWrapper