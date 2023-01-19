#pragma once
#include "vulkan/vulkan.hpp"

namespace vulkan
{
class DeviceQueue;
struct Buffer
{
	vk::Buffer       buffer;
	vk::DeviceMemory memory;
};

struct AllocatedBuffer
{
	vk::Buffer       buffer{};
	vk::DeviceMemory memory;
	vk::DeviceSize   size{};
	vk::DeviceSize   offset{};
};

struct Texture
{
	vk::Image        image;
	vk::DeviceMemory memory;
	vk::ImageView    imageView;
	vk::Sampler      sampler;

	vk::Result Create(vulkan::DeviceQueue& devices, vk::CommandPool& commandPool, vk::Queue queue, const void* data, uint32_t width, uint32_t height);

	vk::Result Load(vulkan::DeviceQueue& devices, vk::CommandPool& commandPool, vk::Queue queue, const char* dir);

	vk::Result destroy(vk::Device& device);
};
} // namespace vulkan