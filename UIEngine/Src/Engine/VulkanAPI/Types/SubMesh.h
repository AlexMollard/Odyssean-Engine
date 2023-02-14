#pragma once
#include "VulkanMaterial.h"
#include "common.h"

namespace vk
{
class PipelineLayout;
class DescriptorSet;
} // namespace vk

namespace VulkanWrapper
{
class CommandBuffer;
struct Buffer;
class DescriptorManager;
} // namespace VulkanWrapper

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
	static vk::VertexInputBindingDescription GetBindingDescription();
};

struct ModelViewProjection
{
	glm::mat4 mvp;
};

class SubMesh
{
public:
	SubMesh()  = default;
	~SubMesh() = default;

	    // Copy constructor that does not transfer ownership of resources
	SubMesh(const SubMesh& other)
	{
		m_vertices       = other.m_vertices;
		m_indices        = other.m_indices;
		m_vertexBuffer   = other.m_vertexBuffer;
		m_indexBuffer    = other.m_indexBuffer;
		m_material       = other.m_material;
		m_tangents       = other.m_tangents;
		m_bitangents     = other.m_bitangents;
		m_descriptorSets = other.m_descriptorSets;
	}

	// Move constructor that transfers ownership of resources
	SubMesh(SubMesh&& other) noexcept
	{
		m_vertices       = std::move(other.m_vertices);
		m_indices        = std::move(other.m_indices);
		m_vertexBuffer   = std::move(other.m_vertexBuffer);
		m_indexBuffer    = std::move(other.m_indexBuffer);
		m_material       = std::move(other.m_material);
		m_tangents       = std::move(other.m_tangents);
		m_bitangents     = std::move(other.m_bitangents);
		m_descriptorSets = std::move(other.m_descriptorSets);
	}

	// Copy assignment operator that does not transfer ownership of resources
	SubMesh& operator=(const SubMesh& other)
	{
		if (this != &other)
		{
			m_vertices       = other.m_vertices;
			m_indices        = other.m_indices;
			m_vertexBuffer   = other.m_vertexBuffer;
			m_indexBuffer    = other.m_indexBuffer;
			m_material       = other.m_material;
			m_tangents       = other.m_tangents;
			m_bitangents     = other.m_bitangents;
			m_descriptorSets = other.m_descriptorSets;
		}
		return *this;
	}

	// Move assignment operator that transfers ownership of resources
	SubMesh& operator=(SubMesh&& other) noexcept
	{
		if (this != &other)
		{
			m_vertices       = std::move(other.m_vertices);
			m_indices        = std::move(other.m_indices);
			m_vertexBuffer   = std::move(other.m_vertexBuffer);
			m_indexBuffer    = std::move(other.m_indexBuffer);
			m_material       = std::move(other.m_material);
			m_tangents       = std::move(other.m_tangents);
			m_bitangents     = std::move(other.m_bitangents);
			m_descriptorSets = std::move(other.m_descriptorSets);
		}
		return *this;
	}

	// Function to create buffers, descriptor sets, and descriptor set layouts
	void CreateBuffers(DeviceQueue& devices);
	void CreateDescriptorSets();

	// Function to update descriptor sets, bind buffers, and add the mesh to a command buffer
	void UpdateDescriptorSets(VulkanWrapper::DescriptorManager* descriptorManager, VulkanWrapper::Buffer* mvpBuffer, VulkanWrapper::Buffer* pointLightsBuffer,
	                          VulkanWrapper::Buffer* directionalLightsBuffer, VulkanWrapper::Buffer* spotLightsBuffer);
	void BindBuffers(vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout);

	// Function to destroy buffers
	void DestroyBuffers();

	// Variables for vertices, indices, and textures
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	VulkanWrapper::Buffer m_vertexBuffer;
	VulkanWrapper::Buffer m_indexBuffer;
	VulkanMaterial m_material;

	// vertex tangents and bitangents
	std::vector<glm::vec3> m_tangents;
	std::vector<glm::vec3> m_bitangents;

private:
	// Variables for descriptor sets
	std::vector<vk::DescriptorSet> m_descriptorSets;
};
} // namespace VulkanWrapper