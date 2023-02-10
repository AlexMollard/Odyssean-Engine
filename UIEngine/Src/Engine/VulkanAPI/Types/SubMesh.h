#pragma once
#include "VulkanMaterial.h"
#include "common.h"
#include <glm/glm.hpp>

namespace vk
{
class PipelineLayout;
}

namespace VulkanWrapper
{
class CommandBuffer;
}

namespace vk
{
class DescriptorSet;
}

namespace VulkanWrapper
{
struct Buffer;
}

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

struct LightProperties
{
	glm::vec3 lightColor;
	glm::vec3 lightPos;
	float lightIntensity;
};

class SubMesh
{
public:
	SubMesh()  = default;
	~SubMesh() = default;

	// Function to create buffers, descriptor sets, and descriptor set layouts
	void CreateBuffers(DeviceQueue& devices);
	void CreateDescriptorSets();

	// Function to update descriptor sets, bind buffers, and add the mesh to a command buffer
	void UpdateDescriptorSets(VulkanWrapper::Buffer* mvpBuffer, VulkanWrapper::Buffer* lightsBuffer);
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