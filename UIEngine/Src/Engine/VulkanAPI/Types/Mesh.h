#pragma once
#include "Container.h"
#include "common.h"
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

struct aiScene;
struct aiNode;
struct aiMesh;
namespace vulkan
{
struct API;
}

namespace vulkan
{
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec3 color;

	// Helper functions to get vertex attributes and binding descriptions
	static std::vector<vk::VertexInputAttributeDescription> GetVertexAttributes();
	static vk::VertexInputBindingDescription                GetBindingDescription();
};

struct LightProperties
{
	glm::vec3 lightPos;
	float spacer;
	glm::vec4 lightColor;
};

struct Mesh
{
	// Destructor
	Mesh() = default;
	void Create(API& api);
	void Destroy(vk::Device& device);

	vk::Result LoadModel(const std::string& path);
	void       ProcessNode(aiNode* node, const aiScene* scene);
	void       ProcessMesh(aiMesh* mesh, const aiScene* scene);

	// Function to add the mesh to a command buffer
	void AddToCommandBuffer(vk::Device& device, vk::CommandBuffer commandBuffer, vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout);

	// Buffer Functions
	void CreateVertexBuffer(DeviceQueue& devices, vk::CommandPool& commandPool);
	void CreateIndexBuffer(DeviceQueue& devices, vk::CommandPool& commandPool);

	// Uniform buffers
	void CreateUBOMatrixBuffer(DeviceQueue& devices, vk::CommandPool& commandPool, glm::mat4 uboMatrix);
	void CreateLightPropertiesBuffer(DeviceQueue& devices, vk::CommandPool& commandPool, const LightProperties& lightProperties);

	// Create Descriptor Functions
	void CreateDescriptorSetLayout(vk::Device& device);
	void CreateDescriptorPool(vk::Device& device);
	void CreateDescriptorSet(DeviceQueue& devices, vulkan::API& api, vk::PipelineLayout& graphicsPipelineLayout);

	// Update Descriptor Functions
	void UpdateUBODescriptorSet(vk::Device& device);
	void UpdateLightPropertiesDescriptorSet(vk::Device& device);

	// Update model stuff
	void UpdateUBOMatrix(vk::Device& device, glm::mat4 uboMatrix);
	void UpdateLightProperties(vk::Device& device, LightProperties lightProperties);

	std::vector<Vertex>     vertices;
	std::vector<uint32_t>   indices;
	AllocatedBuffer         vertexBuffer;
	AllocatedBuffer         indexBuffer;
	
	std::string             directory;
	std::string             texturePath;
	
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorSet       descriptorSet;
	vk::DescriptorPool      descriptorPool;

	vk::DescriptorBufferInfo uboMatrixDescriptorInfo;
	AllocatedBuffer          uboMatrixBuffer;
	glm::mat4                uboMatrix;

	vk::DescriptorImageInfo textureDescriptorInfo;
	Texture                 texture;

	vk::DescriptorBufferInfo lightPropertiesBufferInfo;
	AllocatedBuffer          lightPropertiesBuffer;
	LightProperties          lightProperties;

	vk::CommandBuffer commandBuffer;
};
} // namespace vulkan
