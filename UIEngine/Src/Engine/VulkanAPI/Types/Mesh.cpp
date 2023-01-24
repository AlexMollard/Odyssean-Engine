#include "Mesh.h"

#include "../Helpers/MeshHelper.h"
#include "../VulkanInit.h"
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
	for (auto& subMesh : m_subMeshes) { subMesh.CreateBuffers(devices); }
}

void Mesh::FreeBuffers()
{
	for (auto& subMesh : m_subMeshes) { subMesh.DestroyBuffers(*m_descriptorManager, m_device); }

	m_device.destroyBuffer(m_mvpBuffer.buffer);
	m_device.freeMemory(m_mvpBuffer.memory);
	m_device.destroyBuffer(m_lightPropertiesBuffer.buffer);
	m_device.freeMemory(m_lightPropertiesBuffer.memory);
}

void Mesh::AllocateDescriptors(VulkanWrapper::VkContainer& api)
{
	// Create the mvp and light buffers
	CreateMVPBuffer(api.deviceQueue);
	CreateLightPropertiesBuffer(api.deviceQueue);

	// Allocate the descriptor sets for each sub mesh
	for (auto& subMesh : m_subMeshes) { subMesh.CreateDescriptorSets(*m_descriptorManager); }
}

std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> Mesh::GetAllDescriptorSetLayouts()
{
	std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> layouts;

	layouts.push_back(m_descriptorManager->getLayout("MVPLayout"));
	layouts.push_back(m_descriptorManager->getLayout("LightsLayout"));
	layouts.push_back(m_descriptorManager->getLayout("MaterialLayout"));

	return layouts;
}

void Mesh::UpdateBuffers(const ModelViewProjection& mvp, const LightProperties& properties)
{
	m_mvpBuffer.Update(m_device, &mvp, sizeof(ModelViewProjection));
	m_lightPropertiesBuffer.Update(m_device, &properties, sizeof(LightProperties));

	for (auto& subMesh : m_subMeshes) { subMesh.UpdateDescriptorSets(m_device, &m_mvpBuffer, &m_lightPropertiesBuffer); }
}

void Mesh::BindForDrawing(VulkanWrapper::VkContainer& api, vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout)
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
	ModelViewProjection mvp;
	m_mvpBuffer = devices.CreateBuffer(vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &mvp, sizeof(ModelViewProjection));
	return m_mvpBuffer;
}

VulkanWrapper::Buffer Mesh::CreateLightPropertiesBuffer(VulkanWrapper::DeviceQueue& devices)
{
	LightProperties lightProps;
	m_lightPropertiesBuffer = devices.CreateBuffer(vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &lightProps, sizeof(LightProperties));
	return m_lightPropertiesBuffer;
}

void Mesh::SetSubMeshes(const std::vector<VulkanWrapper::SubMesh>& subMeshes)
{
	m_subMeshes = subMeshes;
}
} // namespace VulkanWrapper