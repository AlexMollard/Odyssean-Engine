#include "pch.h"

#include "Mesh.h"

#include "../DescriptorManager.h"
#include "../Helpers/MeshHelper.h"
#include "../VulkanInit.h"
#include "common.h"
#include <iostream>

namespace VulkanWrapper
{
void Mesh::Create(VkContainer& api, DescriptorManager& descriptorManager)
{
	Initialize(api);
	AllocateBuffers(api.deviceQueue);
	AllocateDescriptors(api);
}

void Mesh::Destroy()
{
	FreeBuffers();
}

vk::Result Mesh::LoadModel(VulkanWrapper::VkContainer& api, const std::string& path)
{
	m_subMeshes = MeshHelper::LoadModel(path);
	MeshHelper::CreateMesh(api, m_subMeshes, *this);

	return vk::Result::eSuccess;
}

void Mesh::Initialize(VkContainer& api)
{
	// Any other crap before stuff gets going goes here
}

void Mesh::AllocateBuffers(DeviceQueue& devices)
{
	for (auto& subMesh : m_subMeshes)
	{
		subMesh.CreateBuffers(devices);
	}
}

void Mesh::FreeBuffers()
{
	for (auto& subMesh : m_subMeshes)
	{
		subMesh.DestroyBuffers();
	}

	m_device.destroyBuffer(m_mvpBuffer.buffer);
	m_device.freeMemory(m_mvpBuffer.memory);

	m_device.destroyBuffer(m_pointLightBuffer.buffer);
	m_device.freeMemory(m_pointLightBuffer.memory);

	m_device.destroyBuffer(m_directionalLightBuffer.buffer);
	m_device.freeMemory(m_directionalLightBuffer.memory);

	m_device.destroyBuffer(m_spotLightBuffer.buffer);
	m_device.freeMemory(m_spotLightBuffer.memory);
}

void Mesh::AllocateDescriptors(VulkanWrapper::VkContainer& api)
{
	// Create the mvp and light buffers
	CreateMVPBuffer(api.deviceQueue);
	CreatePointLightBuffer(api.deviceQueue);
	CreateDirectionalLightBuffer(api.deviceQueue);
	CreateSpotLightBuffer(api.deviceQueue);

	// Allocate the descriptor sets for each sub mesh
	for (auto& subMesh : m_subMeshes)
	{
		subMesh.CreateDescriptorSets();
	}
}

std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> Mesh::GetAllDescriptorSetLayouts()
{
	std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> layouts;

	layouts.push_back(m_descriptorManager->getLayout("MVPLayout"));
	layouts.push_back(m_descriptorManager->getLayout("AllLightsLayout"));
	layouts.push_back(m_descriptorManager->getLayout("PBRMaterialLayout"));

	return layouts;
}

void Mesh::UpdateBuffers(const ModelViewProjection& mvp, const std::vector<std::reference_wrapper<const PointLight>>& pointLights,
                         const std::vector<std::reference_wrapper<const DirectionalLight>>& directionalLights,
                         const std::vector<std::reference_wrapper<const SpotLight>>& spotLights)
{
	m_mvpBuffer.Update(m_device, &mvp, vk::DeviceSize{sizeof(ModelViewProjection)});

	m_pointLightBuffer.Update(m_device, &pointLights.data()->get(), vk::DeviceSize{ sizeof(PointLight) * MAX_POINT_LIGHTS });

	//! The other lights are not implemented yet (Directional and Spot)

	for (auto& subMesh : m_subMeshes)
	{
		subMesh.UpdateDescriptorSets(m_descriptorManager, &m_mvpBuffer, &m_pointLightBuffer, &m_directionalLightBuffer, &m_spotLightBuffer);
	}
}

void Mesh::BindForDrawing(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout)
{
	// Draw all sub meshes
	for (auto& submesh : m_subMeshes)
	{
		submesh.BindBuffers(commandBuffer, pipelineLayout);
		commandBuffer.drawIndexed(submesh.m_indices.size(), 1, 0, 0, 0);
	}
}

VulkanWrapper::Buffer Mesh::CreateMVPBuffer(VulkanWrapper::DeviceQueue& devices)
{
	vk::DeviceSize const bufferSize = m_descriptorManager->GetMinUniformBufferSize(sizeof(ModelViewProjection));

	m_mvpBuffer = devices.CreateBuffer(vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, nullptr,
	                                   bufferSize);

	return m_mvpBuffer;
}

VulkanWrapper::Buffer Mesh::CreatePointLightBuffer(VulkanWrapper::DeviceQueue& devices)
{
	size_t const lightSize = m_descriptorManager->GetMinUniformBufferSize(sizeof(PointLight));

	vk::DeviceSize const bufferSize = lightSize * MAX_POINT_LIGHTS;

	m_pointLightBuffer = devices.CreateBuffer(vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	                                          nullptr, bufferSize);

	return m_pointLightBuffer;
}

VulkanWrapper::Buffer Mesh::CreateDirectionalLightBuffer(VulkanWrapper::DeviceQueue& devices)
{
	size_t const lightSize = m_descriptorManager->GetMinUniformBufferSize(sizeof(DirectionalLight));

	vk::DeviceSize const bufferSize = lightSize * MAX_DIRECTIONAL_LIGHTS;

	m_directionalLightBuffer = devices.CreateBuffer(vk::BufferUsageFlagBits::eUniformBuffer,
	                                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, nullptr, bufferSize);

	return m_directionalLightBuffer;
}

VulkanWrapper::Buffer Mesh::CreateSpotLightBuffer(VulkanWrapper::DeviceQueue& devices)
{
	size_t const lightSize = m_descriptorManager->GetMinUniformBufferSize(sizeof(SpotLight));

	vk::DeviceSize const bufferSize = lightSize * MAX_SPOT_LIGHTS;

	m_spotLightBuffer = devices.CreateBuffer(vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	                                         nullptr, bufferSize);

	return m_spotLightBuffer;
}

void Mesh::SetSubMeshes(const std::vector<VulkanWrapper::SubMesh>& subMeshes)
{
	m_subMeshes = subMeshes;
}
} // namespace VulkanWrapper