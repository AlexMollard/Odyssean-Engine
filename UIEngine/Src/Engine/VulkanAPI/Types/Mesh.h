#pragma once
#include "DescriptorSetLayout.h"
#include "SubMesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;

namespace VulkanWrapper
{
class DescriptorManager;
}

namespace VulkanWrapper
{
struct VkContainer;

// Define struct for mesh
struct Mesh
{
	Mesh(vk::Device& device, vk::PhysicalDevice& physicalDevice, DescriptorManager* descriptorManager)
	    : m_device(device), m_physicalDevice(physicalDevice), m_descriptorManager(descriptorManager)
	{
	}
	~Mesh() = default;

	// Functions to create and destroy the mesh
	void Create(VkContainer& api, DescriptorManager& descriptorManager);
	void Destroy();

	// Function to load a model
	vk::Result LoadModel(VulkanWrapper::VkContainer& api, const std::string& path);

	// Helper function to initialize and set up member variables
	void Initialize(VkContainer& api);

	// Helper functions to handle resource allocation and deallocation
	void AllocateBuffers(DeviceQueue& devices);
	void FreeBuffers();
	void AllocateDescriptors(VulkanWrapper::VkContainer& api);

	// Helper function to bind buffers and descriptor sets for
	std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> GetAllDescriptorSetLayouts();

	void UpdateBuffers(VulkanWrapper::VkContainer& api, const ModelViewProjection& mvp, const LightProperties& properties);
	void BindForDrawing(VulkanWrapper::VkContainer& api, vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout);
	void SetSubMeshes(const std::vector<VulkanWrapper::SubMesh>& subMeshes);

	std::string m_directory;

private:
	vk::Device m_device;
	vk::PhysicalDevice m_physicalDevice;

	std::vector<SubMesh> m_subMeshes;

	// MVP and light properties buffer and descriptor set
	VulkanWrapper::Buffer m_mvpBuffer;
	VulkanWrapper::Buffer m_lightPropertiesBuffer;

	DescriptorManager* m_descriptorManager;

	vk::DescriptorSet m_mvpDescriptorSet;
	vk::DescriptorSet m_lightPropertiesDescriptorSet;

	// Buffer creation
	VulkanWrapper::Buffer CreateMVPBuffer(VulkanWrapper::DeviceQueue& devices);
	VulkanWrapper::Buffer CreateLightPropertiesBuffer(VulkanWrapper::DeviceQueue& devices);
};
} // namespace VulkanWrapper
