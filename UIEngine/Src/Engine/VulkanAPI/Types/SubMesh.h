#pragma once
#include "Material.h"
#include "VkContainer.h"
#include <glm/glm.hpp>

namespace VulkanWrapper
{
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec4 color;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	// Helper functions to get vertex attributes and binding descriptions
	static std::vector<vk::VertexInputAttributeDescription> GetVertexAttributes();
	static vk::VertexInputBindingDescription                GetBindingDescription();
};

struct ModelViewProjection
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

struct LightProperties
{
	glm::vec4 lightColor;
	glm::vec3 lightPos;
};

class SubMesh
{
public:
	SubMesh()  = default;
	~SubMesh() = default;

	// Function to create buffers, descriptor sets, and descriptor set layouts
	void CreateBuffers(DeviceQueue& devices);
	void CreateDescriptorSets(VulkanWrapper::DescriptorManager& descriptorManager);

	// Function to update descriptor sets, bind buffers, and add the mesh to a command buffer
	void UpdateDescriptorSets(vk::Device& device, VulkanWrapper::Buffer* mvpBuffer, VulkanWrapper::Buffer* lightsBuffer);
	void BindBuffers(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout);

	// Function to destroy buffers
	void DestroyBuffers(VulkanWrapper::DescriptorManager& descManager, vk::Device& device);

	// Variables for vertices, indices, and textures
	std::vector<Vertex>   m_vertices;
	std::vector<uint32_t> m_indices;
	VulkanWrapper::Buffer m_vertexBuffer;
	VulkanWrapper::Buffer m_indexBuffer;
	Material              m_material;

	// vertex tangents and bitangents
	std::vector<glm::vec3> m_tangents;
	std::vector<glm::vec3> m_bitangents;

private:
	// Variables for descriptor sets
	std::vector<vk::DescriptorSet> m_descriptorSets;
};
} // namespace VulkanWrapper