#pragma once

#include "VkTypes.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace vulkan
{
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec3 color;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	AllocatedBuffer vertexBuffer;
	AllocatedBuffer indexBuffer;

	std::string directory;
	std::string texturePath;

	// Descriptor Set Layout
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorSet descriptorSet; 
	vk::DescriptorPool descriptorPool;

	// Buffer info (uniform and texture)
	AllocatedBuffer modelMatrixBuffer;
	vk::DescriptorBufferInfo modelMatrixDescriptorInfo;
	vk::DescriptorImageInfo textureDescriptorInfo;

	// Texture
	Texture texture;

	// Destructor
	void Destroy(VkDevice& device)
	{
		// Free the buffers
		vkDestroyBuffer(device, vertexBuffer.buffer, nullptr);
		vkFreeMemory(device, vertexBuffer.memory, nullptr);

		vkDestroyBuffer(device, indexBuffer.buffer, nullptr);
		vkFreeMemory(device, indexBuffer.memory, nullptr);
	}

	void AddToCommandBuffer(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout)
	{
		// Bind the descriptor set
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);

		// Bind the vertex and index buffers
		vk::DeviceSize offsets[] = { 0 };
		commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
		commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);

		// Draw the mesh
		commandBuffer.drawIndexed(indices.size(), 1, 0, 0, 0);
	}
	
	glm::mat4 modelMatrix;

};

} // namespace vulkan