#pragma once
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace VulkanWrapper
{
class DescriptorSetLayout
{
public:
	DescriptorSetLayout(vk::Device device, const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
	~DescriptorSetLayout();

	DescriptorSetLayout(const DescriptorSetLayout& other) = delete;
	DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
	DescriptorSetLayout& operator=(const DescriptorSetLayout& other) = delete;
	DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;

	bool operator==(const DescriptorSetLayout& other) const;

	vk::DescriptorSetLayout GetLayout() const;
	const std::vector<vk::DescriptorSetLayoutBinding>& GetBindings() const;

	// Create descriptor sets
	std::vector<vk::DescriptorSet> CreateDescriptorSets(vk::DescriptorPool descriptorPool, uint32_t count);

	// DEBUGGING
	void Print() const;

private:
	vk::Device m_device;
	vk::DescriptorSetLayout m_layout;

	std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
};
} // namespace VulkanWrapper