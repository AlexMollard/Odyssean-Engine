#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanWrapper
{
class BindingData
{
public:
	BindingData(uint32_t binding, vk::DescriptorType descriptorType, uint32_t descriptorCount, vk::ShaderStageFlags stageFlags);

	vk::DescriptorSetLayoutBinding createDescriptorSetLayoutBinding();

	vk::DescriptorPoolSize createDescriptorPoolSize();

	uint32_t getBinding() const;
	vk::DescriptorType getDescriptorType() const;
	uint32_t getDescriptorCount() const;
	vk::ShaderStageFlags getStageFlags() const;

private:
	uint32_t m_binding;
	vk::DescriptorType m_descriptorType;
	uint32_t m_descriptorCount;
	vk::ShaderStageFlags m_stageFlags;
};
} // namespace VulkanWrapper