#include "VulkanMaterial.h"

#include "SubMesh.h"
#include <assimp/scene.h>
#include <vulkan/vulkan.hpp>

int VulkanWrapper::VulkanMaterial::GetTextureCount()
{
	return m_Textures.size();
}

void VulkanWrapper::VulkanMaterial::ProcessMaterial(aiMaterial* material, SubMesh& subMesh)
{
	aiString name;
	material->Get(AI_MATKEY_NAME, name);
	subMesh.m_material.m_Name = name.C_Str();

	// loop all the aiTextureType and get the texture path
	aiString path;
	for (int i = 0; i < aiTextureType_UNKNOWN; i++)
	{
		if (material->GetTexture((aiTextureType)i, 0, &path) == AI_SUCCESS) { subMesh.m_material.m_Textures[(aiTextureType)i] = path.C_Str(); }
	}
}

std::string& VulkanWrapper::VulkanMaterial::operator[](int index)
{
	return m_Textures[(aiTextureType)index];
}

std::string& VulkanWrapper::VulkanMaterial::operator[](aiTextureType type)
{
	return m_Textures[type];
}

bool VulkanWrapper::VulkanMaterial::hasTexture(aiTextureType type)
{
	return m_Textures[type].empty();
}

VulkanWrapper::VulkanMaterial VulkanWrapper::VulkanMaterial::CreateDebugMaterial(VulkanWrapper::VkContainer& api)
{
	VulkanMaterial material;
	
	material.m_Name                                      = "DebugMaterial";
	material.m_Textures[aiTextureType_DIFFUSE]           = "../Resources/Images/debug_diffuse.png";
	material.m_Textures[aiTextureType_NORMALS]           = "../Resources/Images/debug_normal.jpg";
	material.m_Textures[aiTextureType_DIFFUSE_ROUGHNESS] = "../Resources/Images/debug_normal.jpg";
	material.m_Textures[aiTextureType_METALNESS]         = "../Resources/Images/debug_normal.jpg";
	material.m_Textures[aiTextureType_AMBIENT]           = "../Resources/Images/debug_diffuse.png";

	material.m_DescriptorImageInfos = material.GetDescriptorData(api);
	return material;
}

const std::vector<const vk::DescriptorImageInfo*>& VulkanWrapper::VulkanMaterial::GetDescriptorData(VulkanWrapper::VkContainer& api)
{
	if (!m_DescriptorImageInfos.empty()) return m_DescriptorImageInfos;

	for (int i = 0; i < aiTextureType_UNKNOWN; i++)
	{
		if (m_Textures[(aiTextureType)i].empty()) continue;
		VulkanWrapper::Texture* currentTex = Texture::Load(api, api.deviceQueue.GetQueue(QueueType::GRAPHICS), m_Textures[(aiTextureType)i].c_str());
		m_DescriptorImageInfos.push_back(&currentTex->GetDescriptorImageInfo());
	}

	return m_DescriptorImageInfos;
}