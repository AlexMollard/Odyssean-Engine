#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanWrapper
{
struct VkContainer;
}

// Error checking
#define VK_CHECK_RESULT(f)                                                                \
	{                                                                                     \
		vk::Result res = (f);                                                             \
		if (res != vk::Result::eSuccess)                                                  \
		{                                                                                 \
			printf("Fatal : VkResult is %d in %s at line %d\n", res, __FILE__, __LINE__); \
			assert(res == vk::Result::eSuccess);                                          \
		}                                                                                 \
	}

namespace VulkanWrapper
{
class DeviceQueue;
struct Buffer
{
	vk::Buffer               buffer;
	vk::DeviceMemory         memory;

	vk::MappedMemoryRange    Update(vk::Device& device, const void* data, size_t size);
};

struct Texture
{
	vk::Image               image;
	vk::DeviceMemory        memory;
	vk::ImageView           imageView;
	vk::Sampler             sampler;
	vk::DescriptorImageInfo descriptorImageInfo;

	void                           Create(VulkanWrapper::VkContainer& api, vk::Queue queue, const void* data, uint32_t width, uint32_t height);
	static VulkanWrapper::Texture* Load(VulkanWrapper::VkContainer& api, vk::Queue queue, const char* dir);

	vk::Result                     destroy(vk::Device& device);
	const vk::DescriptorImageInfo& GetDescriptorImageInfo() const;
};
} // namespace VulkanWrapper