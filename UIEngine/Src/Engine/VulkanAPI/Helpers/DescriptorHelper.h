#pragma once
#include "vulkan/vulkan.hpp"

class DescriptorHelper
{
public:
	// Helper function to create a descriptor set layout
	static vk::DescriptorSetLayout CreateDescriptorSetLayout(vk::Device& device, const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
	{
		vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings.size(), bindings.data());
		return device.createDescriptorSetLayout(layoutInfo);
	}

	// Helper function to allocate descriptor sets
	static std::vector<vk::DescriptorSet> AllocateDescriptorSets(vk::Device& device, vk::DescriptorPool& descriptorPool, const std::vector<vk::DescriptorSetLayout>& setLayouts)
	{
		vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, setLayouts.size(), setLayouts.data());
		return device.allocateDescriptorSets(allocInfo);
	}

	// Helper function to update descriptor sets
	static void UpdateDescriptorSets(vk::Device& device, const std::vector<vk::WriteDescriptorSet>& descriptorWrites, const std::vector<vk::CopyDescriptorSet>& descriptorCopies = {})
	{
		device.updateDescriptorSets(descriptorWrites, descriptorCopies);
	}

	// Helper function to create descriptor set writes
	static vk::WriteDescriptorSet CreateDescriptorWrite(vk::DescriptorSet& descriptorSet, uint32_t binding, vk::DescriptorType descriptorType, uint32_t descriptorCount, vk::ShaderStageFlags stageFlags,
														const vk::DescriptorBufferInfo* bufferInfo = nullptr, const vk::DescriptorImageInfo* imageInfo = nullptr)
	{
		return {descriptorSet, binding, 0, descriptorCount, descriptorType, imageInfo, bufferInfo, nullptr};
	}
};