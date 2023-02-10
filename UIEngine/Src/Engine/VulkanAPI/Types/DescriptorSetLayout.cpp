#include "pch.h"

#include "DescriptorSetLayout.h"

#include <iostream>

VulkanWrapper::DescriptorSetLayout::DescriptorSetLayout(vk::Device device, const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
    : m_device(device), m_bindings(bindings)
{
	vk::DescriptorSetLayoutCreateInfo createInfo = vk::DescriptorSetLayoutCreateInfo().setBindingCount(static_cast<uint32_t>(bindings.size())).setPBindings(bindings.data());

	m_layout = device.createDescriptorSetLayout(createInfo);
}

VulkanWrapper::DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
    : m_device(other.m_device), m_layout(other.m_layout), m_bindings(std::move(other.m_bindings))
{
	other.m_layout = nullptr;
}

VulkanWrapper::DescriptorSetLayout& VulkanWrapper::DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept
{
	if (this != &other)
	{
		if (m_layout)
		{
			m_device.destroyDescriptorSetLayout(m_layout);
		}

		m_device   = other.m_device;
		m_layout   = other.m_layout;
		m_bindings = std::move(other.m_bindings);

		other.m_layout = nullptr;
	}

	return *this;
}

const std::vector<vk::DescriptorSetLayoutBinding>& VulkanWrapper::DescriptorSetLayout::GetBindings() const
{
	return m_bindings;
}

VulkanWrapper::DescriptorSetLayout::~DescriptorSetLayout()
{
	m_device.destroyDescriptorSetLayout(m_layout);
}

vk::DescriptorSetLayout VulkanWrapper::DescriptorSetLayout::GetLayout() const
{
	return m_layout;
}

std::vector<vk::DescriptorSet> VulkanWrapper::DescriptorSetLayout::CreateDescriptorSets(vk::DescriptorPool descriptorPool, uint32_t count)
{
	std::vector<vk::DescriptorSet> descriptorSets(count);

	std::vector<vk::DescriptorSetLayout> layouts(count, m_layout);

	vk::DescriptorSetAllocateInfo allocateInfo = vk::DescriptorSetAllocateInfo().setDescriptorPool(descriptorPool).setDescriptorSetCount(count).setPSetLayouts(layouts.data());

	m_device.allocateDescriptorSets(&allocateInfo, descriptorSets.data());

	return descriptorSets;
}

bool VulkanWrapper::DescriptorSetLayout::operator==(const DescriptorSetLayout& other) const
{
	// Compare member variables here
	return m_bindings == other.m_bindings;
}

void VulkanWrapper::DescriptorSetLayout::Print() const
{
	std::cout << "\033[1;34mDescriptor Set Layout\033[0m: " << this->GetLayout() << std::endl;
	for (const auto& binding : m_bindings)
	{
		std::cout << " \033[1;31mBinding\033[0m: " << binding.binding << std::endl;
		std::cout << "  DescriptorType: " << vk::to_string(binding.descriptorType) << std::endl;
		std::cout << "  DescriptorCount: " << binding.descriptorCount << std::endl;
		std::cout << "  StageFlags: " << vk::to_string(binding.stageFlags) << std::endl;
		std::cout << "  ImmutableSamplers: " << binding.pImmutableSamplers << std::endl;
		std::cout << std::endl;
	}
}