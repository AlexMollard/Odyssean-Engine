#pragma once

#include "Types/DescriptorLayoutCache.h"
#include <map>
#include <vector>

namespace VulkanWrapper
{
class DescriptorManager
{
public:
	DescriptorManager(vk::Device device);
	~DescriptorManager() = default;

	size_t GetMinUniformBufferSize(size_t size);
	size_t GetMinUniformBufferOffsetAlignment();

	// Function to create a descriptor set layout
	std::shared_ptr<VulkanWrapper::DescriptorSetLayout> createDescriptorSetLayout(std::vector<VulkanWrapper::BindingData> bindings);

	// Function to allocate descriptor sets
	std::vector<vk::DescriptorSet> allocateDescriptorSets(VulkanWrapper::DescriptorSetLayout descriptorSetLayout, uint32_t descriptorSetCount);
	std::vector<vk::DescriptorSet> allocateDescriptorSets(std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> descriptorSetLayouts, uint32_t descriptorSetCount);

	// Function to update descriptor sets
	void updateDescriptorSets(std::vector<vk::WriteDescriptorSet> descriptorWrites);

	void cleanup();

	std::shared_ptr<VulkanWrapper::DescriptorSetLayout> getLayout(const char* name);

	// Frees all descriptor sets passed to it
	void freeDescriptorSets(std::vector<vk::DescriptorSet> descriptorSets);

	// STATIC FUNCTIONS
	static std::vector<vk::DescriptorSetLayout> GetLayoutData(const std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>>& descriptorSetLayouts);

	static void DebugLayouts(std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> descriptorSetLayouts);

private:
	void createBaseDescriptorLayouts();
	size_t setMinUniformBufferOffsetAlignment();

	vk::Device m_device;
	DescriptorLayoutCache m_descriptorLayoutCache;

	std::vector<vk::DescriptorSet> m_descriptorSets;
	vk::DescriptorPool m_descriptorPool;

	size_t m_minUniformBufferOffsetAlignment;
};
} // namespace VulkanWrapper
