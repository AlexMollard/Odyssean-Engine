#include "pch.h"

#include "SubMesh.h"

#include "Engine/VulkanAPI/DescriptorManager.h"
#include "CommandBuffer.h"
#include "DeviceQueue.h"
#include "VkContainer.h"
#include "common.h"
#include <iostream>
#include "assimp/material.h"
#include "VulkanMaterial.h"

std::vector<vk::VertexInputAttributeDescription> VulkanWrapper::Vertex::GetVertexAttributes()
{
	std::vector<vk::VertexInputAttributeDescription> vertexAttributes(6);

	// Vertex Position
	vertexAttributes[0].binding  = 0;
	vertexAttributes[0].location = 0;
	vertexAttributes[0].format   = vk::Format::eR32G32B32Sfloat;
	vertexAttributes[0].offset   = offsetof(Vertex, pos);

	// Vertex Normal
	vertexAttributes[1].binding  = 0;
	vertexAttributes[1].location = 1;
	vertexAttributes[1].format   = vk::Format::eR32G32B32Sfloat;
	vertexAttributes[1].offset   = offsetof(Vertex, normal);

	// Vertex Texture Coordinates
	vertexAttributes[2].binding  = 0;
	vertexAttributes[2].location = 2;
	vertexAttributes[2].format   = vk::Format::eR32G32Sfloat;
	vertexAttributes[2].offset   = offsetof(Vertex, texCoord);

	// Vertex Color
	vertexAttributes[3].binding  = 0;
	vertexAttributes[3].location = 3;
	vertexAttributes[3].format   = vk::Format::eR32G32B32Sfloat;
	vertexAttributes[3].offset   = offsetof(Vertex, color);

	// Tangent
	vertexAttributes[4].binding  = 0;
	vertexAttributes[4].location = 4;
	vertexAttributes[4].format   = vk::Format::eR32G32B32Sfloat;
	vertexAttributes[4].offset   = offsetof(Vertex, tangent);

	// Bitangent
	vertexAttributes[5].binding  = 0;
	vertexAttributes[5].location = 5;
	vertexAttributes[5].format   = vk::Format::eR32G32B32Sfloat;
	vertexAttributes[5].offset   = offsetof(Vertex, bitangent);

	return vertexAttributes;
}

vk::VertexInputBindingDescription VulkanWrapper::Vertex::GetBindingDescription()
{
	vk::VertexInputBindingDescription bindingDescription;
	bindingDescription.binding   = 0;
	bindingDescription.stride    = sizeof(Vertex);
	bindingDescription.inputRate = vk::VertexInputRate::eVertex;
	return bindingDescription;
}

void VulkanWrapper::SubMesh::CreateBuffers(DeviceQueue& devices)
{
	// Create vertex buffer
	vk::BufferCreateInfo vertexBufferInfo;
	vertexBufferInfo.size  = m_vertices.size() * sizeof(Vertex);
	vertexBufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;

	m_vertexBuffer.buffer = devices.m_Device.createBuffer(vertexBufferInfo);

	// Allocate memory for vertex buffer
	vk::MemoryRequirements memoryRequirements = devices.m_Device.getBufferMemoryRequirements(m_vertexBuffer.buffer);
	vk::MemoryAllocateInfo vertexAllocInfo;

	vertexAllocInfo.allocationSize = memoryRequirements.size;
	uint32_t memoryTypeBits        = memoryRequirements.memoryTypeBits;

	vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	devices.FindMemoryType(memoryTypeBits, properties, vertexAllocInfo.memoryTypeIndex);

	vk::DeviceMemory vertexBufferMemory = devices.m_Device.allocateMemory(vertexAllocInfo);
	m_vertexBuffer.memory               = vertexBufferMemory;

	// Bind vertex buffer memory
	devices.m_Device.bindBufferMemory(m_vertexBuffer.buffer, vertexBufferMemory, 0);

	// Copy vertex data to vertex buffer memory
	void* data = devices.m_Device.mapMemory(vertexBufferMemory, 0, vertexBufferInfo.size);
	memcpy(data, m_vertices.data(), vertexBufferInfo.size);
	devices.m_Device.unmapMemory(vertexBufferMemory);

	// Create index buffer
	vk::BufferCreateInfo indexBufferInfo;
	indexBufferInfo.size  = m_indices.size() * sizeof(uint32_t);
	indexBufferInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;

	m_indexBuffer.buffer = devices.m_Device.createBuffer(indexBufferInfo);

	// Allocate memory for buffer
	memoryRequirements = devices.m_Device.getBufferMemoryRequirements(m_indexBuffer.buffer);

	vk::MemoryAllocateInfo indexAllocInfo;
	indexAllocInfo.allocationSize = memoryRequirements.size;
	memoryTypeBits                = memoryRequirements.memoryTypeBits;
	devices.FindMemoryType(memoryTypeBits, properties, indexAllocInfo.memoryTypeIndex);

	vk::DeviceMemory indexBufferMemory = devices.m_Device.allocateMemory(indexAllocInfo);
	m_indexBuffer.memory               = indexBufferMemory;

	// Bind index buffer memory
	devices.m_Device.bindBufferMemory(m_indexBuffer.buffer, indexBufferMemory, 0);

	// Copy index data to index buffer memory
	data = devices.m_Device.mapMemory(indexBufferMemory, 0, indexBufferInfo.size);
	memcpy(data, m_indices.data(), indexBufferInfo.size);
	devices.m_Device.unmapMemory(indexBufferMemory);
}

void VulkanWrapper::SubMesh::CreateDescriptorSets()
{
	VulkanWrapper::VkContainer& api = VulkanWrapper::VkContainer::instance();

	// Get descriptor set layouts
	std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> layouts;

	auto MVPLayout      = api.descriptorManager->getLayout("MVPLayout");
	auto AllLightLayout = api.descriptorManager->getLayout("AllLightsLayout");
	auto MaterialLayout = api.descriptorManager->getLayout("MaterialLayout");

	// Vertex Shader
	layouts.push_back(MVPLayout);

	// Fragment Shader
	layouts.push_back(AllLightLayout);
	layouts.push_back(MaterialLayout);

	if (m_material.m_Textures[aiTextureType_DIFFUSE].empty())
	{
		VulkanWrapper::VulkanMaterial newMaterial = VulkanMaterial::CreateDebugMaterial(api);

		for (int i = 0; i < 13; i++)
		{
			if (m_material.m_Textures[(aiTextureType)i].empty())
			{
				m_material.m_Textures[(aiTextureType)i] = newMaterial[(aiTextureType)i];
			}
		}
	}

	// Allocate descriptor sets
	m_descriptorSets = api.descriptorManager->allocateDescriptorSets(layouts, 1);

	// Material
	std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
	std::vector<const vk::DescriptorImageInfo*> descriptorImageInfos = m_material.GetDescriptorData(api);

	writeDescriptorSets.reserve(descriptorImageInfos.size());

	// Iterate through the descriptorImageInfos and create a write descriptor set for each one
	for (uint32_t i = 0; i < descriptorImageInfos.size(); ++i)
	{
		vk::WriteDescriptorSet writeDescriptorSet;
		writeDescriptorSet.dstSet          = m_descriptorSets[2];
		writeDescriptorSet.dstBinding      = i;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.pImageInfo      = descriptorImageInfos[i];

		writeDescriptorSets.push_back(writeDescriptorSet);
	}

	api.device.updateDescriptorSets(writeDescriptorSets, nullptr);
}

void VulkanWrapper::SubMesh::UpdateDescriptorSets(VulkanWrapper::DescriptorManager* descriptorManager, VulkanWrapper::Buffer* mvpBuffer,
                                                  VulkanWrapper::Buffer* pointLightsBuffer, VulkanWrapper::Buffer* directionalLightsBuffer,
                                                  VulkanWrapper::Buffer* spotLightsBuffer)
{
	S_WARN(mvpBuffer || mvpBuffer->buffer, "Error: MVP buffer is not valid.")
	S_WARN(pointLightsBuffer || pointLightsBuffer->buffer, "Error: Point lights buffer is not valid.")
	S_WARN(directionalLightsBuffer || directionalLightsBuffer->buffer, "Error: Directional lights buffer is not valid.")
	S_WARN(spotLightsBuffer || spotLightsBuffer->buffer, "Error: Spot lights buffer is not valid.")

	// Update descriptor sets
	std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

	// MVP
	vk::DescriptorBufferInfo mvpBufferInfo;
	mvpBufferInfo.buffer = mvpBuffer->buffer;
	mvpBufferInfo.offset = 0;
	mvpBufferInfo.range  = mvpBuffer->bufferSize;

	// Lights
	vk::DescriptorBufferInfo pointLightsBufferInfo;
	pointLightsBufferInfo.buffer = pointLightsBuffer->buffer;
	pointLightsBufferInfo.offset = 0;
	pointLightsBufferInfo.range  = pointLightsBuffer->bufferSize;

	//vk::DescriptorBufferInfo directionalLightsBufferInfo;
	//directionalLightsBufferInfo.buffer = directionalLightsBuffer->buffer;
	//directionalLightsBufferInfo.offset = 0;
	//directionalLightsBufferInfo.range  = directionalLightsBuffer->bufferSize;

	//vk::DescriptorBufferInfo spotLightsBufferInfo;
	//spotLightsBufferInfo.buffer = spotLightsBuffer->buffer;
	//spotLightsBufferInfo.offset = 0;
	//spotLightsBufferInfo.range  = spotLightsBuffer->bufferSize;

	// MVP
	vk::WriteDescriptorSet mvpWriteDescriptorSet;
	mvpWriteDescriptorSet.dstSet          = m_descriptorSets[0];
	mvpWriteDescriptorSet.dstBinding      = 0;
	mvpWriteDescriptorSet.dstArrayElement = 0;
	mvpWriteDescriptorSet.descriptorType  = vk::DescriptorType::eUniformBuffer;
	mvpWriteDescriptorSet.descriptorCount = 1;
	mvpWriteDescriptorSet.pBufferInfo     = &mvpBufferInfo;

	// Lights
	vk::WriteDescriptorSet pointLightsWriteDescriptorSet;
	pointLightsWriteDescriptorSet.dstSet          = m_descriptorSets[1];
	pointLightsWriteDescriptorSet.dstBinding      = 0;
	pointLightsWriteDescriptorSet.dstArrayElement = 0;
	pointLightsWriteDescriptorSet.descriptorType  = vk::DescriptorType::eStorageBuffer;
	pointLightsWriteDescriptorSet.descriptorCount = 1;
	pointLightsWriteDescriptorSet.pBufferInfo     = &pointLightsBufferInfo;

	//vk::WriteDescriptorSet directionalLightsWriteDescriptorSet;
	//directionalLightsWriteDescriptorSet.dstSet          = m_descriptorSets[1];
	//directionalLightsWriteDescriptorSet.dstBinding      = 1;
	//directionalLightsWriteDescriptorSet.dstArrayElement = 0;
	//directionalLightsWriteDescriptorSet.descriptorType  = vk::DescriptorType::eUniformBuffer;
	//directionalLightsWriteDescriptorSet.descriptorCount = 1;
	//directionalLightsWriteDescriptorSet.pBufferInfo     = &directionalLightsBufferInfo;

	//vk::WriteDescriptorSet spotLightsWriteDescriptorSet;
	//spotLightsWriteDescriptorSet.dstSet          = m_descriptorSets[1];
	//spotLightsWriteDescriptorSet.dstBinding      = 2;
	//spotLightsWriteDescriptorSet.dstArrayElement = 0;
	//spotLightsWriteDescriptorSet.descriptorType  = vk::DescriptorType::eUniformBuffer;
	//spotLightsWriteDescriptorSet.descriptorCount = 1;
	//spotLightsWriteDescriptorSet.pBufferInfo     = &spotLightsBufferInfo;

	writeDescriptorSets.push_back(mvpWriteDescriptorSet);
	writeDescriptorSets.push_back(pointLightsWriteDescriptorSet);
	// writeDescriptorSets.push_back(directionalLightsWriteDescriptorSet);
	// writeDescriptorSets.push_back(spotLightsWriteDescriptorSet);

	VulkanWrapper::VkContainer::instance().device.updateDescriptorSets(writeDescriptorSets, nullptr);
}

void VulkanWrapper::SubMesh::BindBuffers(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout)
{
	// Bind vertex buffer
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, &m_vertexBuffer.buffer, offsets);

	// Bind index buffer
	commandBuffer.bindIndexBuffer(m_indexBuffer.buffer, 0, vk::IndexType::eUint32);

	// Bind descriptor sets
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, m_descriptorSets.size(), m_descriptorSets.data(), 0, nullptr);
}

void VulkanWrapper::SubMesh::DestroyBuffers()
{
	const vk::Device& device = VulkanWrapper::VkContainer::instance().device;

	device.destroyBuffer(m_vertexBuffer.buffer);
	device.freeMemory(m_vertexBuffer.memory);

	device.destroyBuffer(m_indexBuffer.buffer);
	device.freeMemory(m_indexBuffer.memory);
}