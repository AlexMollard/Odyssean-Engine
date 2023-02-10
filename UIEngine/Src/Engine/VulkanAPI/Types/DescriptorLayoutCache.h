#pragma once
#include "BindingData.h"
#include "DescriptorSetLayout.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace VulkanWrapper
{
struct DescriptorSetLayoutBindingsHash
{
	std::size_t operator()(const std::vector<vk::DescriptorSetLayoutBinding>& bindings) const noexcept
	{
		std::size_t seed = 0;
		for (const auto& binding : bindings)
		{
			seed ^= std::hash<uint32_t>{}(binding.binding) ^ (std::hash<vk::DescriptorType>{}(binding.descriptorType) << 1);
			seed ^= (std::hash<uint32_t>{}(binding.descriptorCount) << 1);

			uint32_t bitmask = 0;
			if (binding.stageFlags & vk::ShaderStageFlagBits::eVertex)
				bitmask |= 1;
			if (binding.stageFlags & vk::ShaderStageFlagBits::eFragment)
				bitmask |= 2;
			if (binding.stageFlags & vk::ShaderStageFlagBits::eCompute)
				bitmask |= 4;
			seed ^= (std::hash<uint32_t>{}(bitmask) << 1);
		}
		return seed;
	}
};

struct DescriptorSetLayoutBindingsEqual
{
	bool operator()(const std::vector<vk::DescriptorSetLayoutBinding>& lhs, const std::vector<vk::DescriptorSetLayoutBinding>& rhs) const noexcept
	{
		return std::is_permutation(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
};

class DescriptorLayoutCache
{
public:
	DescriptorLayoutCache(vk::Device device);
	std::shared_ptr<VulkanWrapper::DescriptorSetLayout> getDescriptorSetLayout(std::vector<BindingData> bindings);

	std::shared_ptr<VulkanWrapper::DescriptorSetLayout> getDescriptorSetLayout(const std::string& layoutName);

	void addDescriptorSetLayout(const std::string& layoutName, std::shared_ptr<VulkanWrapper::DescriptorSetLayout> layout);

private:
	vk::Device m_device;

	std::unordered_map<std::vector<vk::DescriptorSetLayoutBinding>, std::weak_ptr<VulkanWrapper::DescriptorSetLayout>, DescriptorSetLayoutBindingsHash,
	                   DescriptorSetLayoutBindingsEqual>
	    m_layouts;
	std::unordered_map<std::string, std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> m_namedLayouts;
};
} // namespace VulkanWrapper