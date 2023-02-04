#include "pch.h"
#include "BindingData.h"

VulkanWrapper::BindingData::BindingData(uint32_t binding, vk::DescriptorType descriptorType, uint32_t descriptorCount, vk::ShaderStageFlags stageFlags)
: m_binding(binding), m_descriptorType(descriptorType), m_descriptorCount(descriptorCount), m_stageFlags(stageFlags)
{}

vk::DescriptorSetLayoutBinding VulkanWrapper::BindingData::createDescriptorSetLayoutBinding()
{
	vk::DescriptorSetLayoutBinding layoutBinding;
	layoutBinding.binding         = m_binding;
	layoutBinding.descriptorType  = m_descriptorType;
	layoutBinding.descriptorCount = m_descriptorCount;
	layoutBinding.stageFlags      = m_stageFlags;
	return layoutBinding;
}

vk::DescriptorPoolSize VulkanWrapper::BindingData::createDescriptorPoolSize()
{
	vk::DescriptorPoolSize poolSize;
	poolSize.type            = m_descriptorType;
	poolSize.descriptorCount = m_descriptorCount;
	return poolSize;
}

uint32_t VulkanWrapper::BindingData::getBinding() const
{
	return m_binding;
}

vk::DescriptorType VulkanWrapper::BindingData::getDescriptorType() const
{
	return m_descriptorType;
}

uint32_t VulkanWrapper::BindingData::getDescriptorCount() const
{
	return m_descriptorCount;
}

vk::ShaderStageFlags VulkanWrapper::BindingData::getStageFlags() const
{
	return m_stageFlags;
}