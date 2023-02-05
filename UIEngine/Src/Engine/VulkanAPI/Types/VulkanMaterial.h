#pragma once
#include <string>
#include <vector>
#include <map>
#include "assimp/material.h"

namespace vk
{
struct DescriptorImageInfo;
}

namespace VulkanWrapper
{
class VkContainer;
class SubMesh;

struct VulkanMaterial
{
	std::string m_Name;

	std::map<aiTextureType, std::string> m_Textures;

	int GetTextureCount();

	const std::vector<const vk::DescriptorImageInfo*>& GetDescriptorData(VulkanWrapper::VkContainer& api);

	static void ProcessMaterial(aiMaterial* material, SubMesh& subMesh);

	std::string& operator[](int index);
	std::string& operator[](aiTextureType type);

	std::vector<const vk::DescriptorImageInfo*> m_DescriptorImageInfos;

	bool hasTexture(aiTextureType type);


	static VulkanMaterial CreateDebugMaterial(VulkanWrapper::VkContainer& api);
};
} // namespace VulkanWrapper