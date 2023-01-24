#include "Material.h"

#include "SubMesh.h"
#include <vulkan/vulkan.hpp>
#include <assimp/scene.h>

int VulkanWrapper::Material::GetTextureCount()
{
	int count = 0;
	if (hasDiffuseMap) count++;
	if (hasSpecularMap) count++;
	if (hasNormalMap) count++;
	if (hasHeightMap) count++;
	if (hasAmbientMap) count++;
	if (hasEmissiveMap) count++;
	if (hasShininessMap) count++;
	if (hasOpacityMap) count++;
	if (hasDisplacementMap) count++;
	if (hasLightMap) count++;
	if (hasReflectionMap) count++;
	if (hasUnknownMap) count++;
	return count;
}

void VulkanWrapper::Material::ProcessMaterial(aiMaterial* material, SubMesh& subMesh)
{
	aiString name;
	material->Get(AI_MATKEY_NAME, name);
	subMesh.m_material.name = name.C_Str();

	aiString path;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.diffuseMap    = path.C_Str();
		subMesh.m_material.hasDiffuseMap = true;
	}
	if (material->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.specularMap    = path.C_Str();
		subMesh.m_material.hasSpecularMap = true;
	}
	if (material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.normalMap    = path.C_Str();
		subMesh.m_material.hasNormalMap = true;
	}
	if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.heightMap    = path.C_Str();
		subMesh.m_material.hasHeightMap = true;
	}
	if (material->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.ambientMap    = path.C_Str();
		subMesh.m_material.hasAmbientMap = true;
	}
	if (material->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.emissiveMap    = path.C_Str();
		subMesh.m_material.hasEmissiveMap = true;
	}
	if (material->GetTexture(aiTextureType_SHININESS, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.shininessMap    = path.C_Str();
		subMesh.m_material.hasShininessMap = true;
	}
	if (material->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.opacityMap    = path.C_Str();
		subMesh.m_material.hasOpacityMap = true;
	}
	if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.displacementMap    = path.C_Str();
		subMesh.m_material.hasDisplacementMap = true;
	}
	if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.lightMap    = path.C_Str();
		subMesh.m_material.hasLightMap = true;
	}
	if (material->GetTexture(aiTextureType_REFLECTION, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.reflectionMap    = path.C_Str();
		subMesh.m_material.hasReflectionMap = true;
	}
	if (material->GetTexture(aiTextureType_UNKNOWN, 0, &path) == AI_SUCCESS)
	{
		subMesh.m_material.unknownMap    = path.C_Str();
		subMesh.m_material.hasUnknownMap = true;
	}
}

const std::vector<const vk::DescriptorImageInfo*>& VulkanWrapper::Material::GetDescriptorData(VulkanWrapper::VkContainer& api)
{
	if (!descriptorImageInfos.empty()) return descriptorImageInfos;

	if (hasDiffuseMap)
	{
		auto texture = Texture::Load(api, api.deviceQueue.GetQueue(QueueType::GRAPHICS), diffuseMap.c_str());
		descriptorImageInfos.push_back(&texture->GetDescriptorImageInfo());
	}
	else
	{
		diffuseMap    = "../Resources/Images/debug.png";
		hasDiffuseMap = true;

		auto texture = Texture::Load(api, api.deviceQueue.GetQueue(QueueType::GRAPHICS), diffuseMap.c_str());
		descriptorImageInfos.push_back(&texture->GetDescriptorImageInfo());
	}

	return descriptorImageInfos;
}

std::string& VulkanWrapper::Material::operator[](int index)
{
	switch (index)
	{
	case 0: return diffuseMap;
	case 1: return specularMap;
	case 2: return normalMap;
	case 3: return heightMap;
	case 4: return ambientMap;
	case 5: return emissiveMap;
	case 6: return shininessMap;
	case 7: return opacityMap;
	case 8: return displacementMap;
	case 9: return lightMap;
	case 10: return reflectionMap;
	case 11: return unknownMap;
	default: return diffuseMap;
	}
}