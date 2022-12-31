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

	// Get the vertex attributes
	static std::vector<vk::VertexInputAttributeDescription> GetVertexAttributes()
	{
		std::vector<vk::VertexInputAttributeDescription> attributes(4);

		// Position
		attributes[0].binding = 0;
		attributes[0].location = 0;
		attributes[0].format = vk::Format::eR32G32B32Sfloat;
		attributes[0].offset = offsetof(Vertex, pos);

		// Normal
		attributes[1].binding = 0;
		attributes[1].location = 1;
		attributes[1].format = vk::Format::eR32G32B32Sfloat;
		attributes[1].offset = offsetof(Vertex, normal);

		// TexCoord
		attributes[2].binding = 0;
		attributes[2].location = 2;
		attributes[2].format = vk::Format::eR32G32Sfloat;
		attributes[2].offset = offsetof(Vertex, texCoord);

		// Color
		attributes[3].binding = 0;
		attributes[3].location = 3;
		attributes[3].format = vk::Format::eR32G32B32Sfloat;
		attributes[3].offset = offsetof(Vertex, color);

		return attributes;
	}

	// Binding description
	static vk::VertexInputBindingDescription GetBindingDescription()
	{
		vk::VertexInputBindingDescription bindingDescription;
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;

		return bindingDescription;
	}
	
	// Destructor
	void Destroy(VkDevice& device)
	{
		vk::Device vk_device = device;

		// Free the buffers
		vk_device.freeMemory(vertexBuffer.memory);
		vk_device.destroyBuffer(vertexBuffer.buffer);
		vk_device.freeMemory(indexBuffer.memory);
		vk_device.destroyBuffer(indexBuffer.buffer);

		// Free the descriptor set
		vk_device.freeMemory(modelMatrixBuffer.memory);
		vk_device.destroyBuffer(modelMatrixBuffer.buffer);
		vk_device.destroyDescriptorPool(descriptorPool);
		vk_device.destroyDescriptorSetLayout(descriptorSetLayout);

		// Destroy the texture
		vk_device.destroyImage(texture.image);
		vk_device.freeMemory(texture.memory);
		vk_device.destroyImageView(texture.imageView);
		vk_device.destroySampler(texture.sampler);
	}

	void AddToCommandBuffer(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout)
	{
		// Bind the vertex and index buffers
		vk::DeviceSize offsets[] = { 0 };
		commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer.buffer, offsets);
		commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);

		// Bind the descriptor set
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		// Draw the mesh
		commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	}

	void UpdateModelMatrix(vk::Device& device, glm::mat4& modelMatrix)
	{
		// Update the model matrix
		this->modelMatrix = modelMatrix;

		// Copy the model matrix to the buffer
		void* data;
		vkMapMemory(device, modelMatrixBuffer.memory, 0, sizeof(glm::mat4), 0, &data);
		memcpy(data, &modelMatrix, sizeof(glm::mat4));
		vkUnmapMemory(device, modelMatrixBuffer.memory);
	}
	
	glm::mat4 modelMatrix;

};

} // namespace vulkan