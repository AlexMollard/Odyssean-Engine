#include "pch.h"

#include "DescriptorLayoutCache.h"

VulkanWrapper::DescriptorLayoutCache::DescriptorLayoutCache(vk::Device device) : m_device(device) {}

std::shared_ptr<VulkanWrapper::DescriptorSetLayout> VulkanWrapper::DescriptorLayoutCache::getDescriptorSetLayout(std::vector<BindingData> bindings)
{
	std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;

	for (auto& binding : bindings)
	{
		layoutBindings.push_back(binding.createDescriptorSetLayoutBinding());
	}

	//std::sort(layoutBindings.begin(), layoutBindings.end(),
	//          [](const vk::DescriptorSetLayoutBinding& a, const vk::DescriptorSetLayoutBinding& b)
	//          {
	//	          if (a.binding != b.binding)
	//	          {
	//		          return a.binding < b.binding;
	//	          }
	//	          if (a.descriptorType != b.descriptorType)
	//	          {
	//		          return a.descriptorType < b.descriptorType;
	//	          }
	//	          return a.descriptorCount < b.descriptorCount;
	//          });

	auto iter = m_layouts.find(layoutBindings);
	if (iter != m_layouts.end())
	{
		auto layout = iter->second.lock();
		if (layout)
		{
			return layout;
		}
	}

	// Create new descriptor set layout
	auto layout = std::make_shared<VulkanWrapper::DescriptorSetLayout>(m_device, layoutBindings);

	m_layouts[layoutBindings] = layout;
	return layout;
}

std::shared_ptr<VulkanWrapper::DescriptorSetLayout> VulkanWrapper::DescriptorLayoutCache::getDescriptorSetLayout(const std::string& layoutName)
{
	auto it = m_namedLayouts.find(layoutName);
	if (it != m_namedLayouts.end())
	{
		return it->second;
	}
	return nullptr;
}

void VulkanWrapper::DescriptorLayoutCache::addDescriptorSetLayout(const std::string& layoutName, std::shared_ptr<VulkanWrapper::DescriptorSetLayout> layout)
{
	m_namedLayouts[layoutName] = layout;
}