#pragma once
#include "vulkan/vulkan.hpp"

namespace vulkan
{
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
};
} // namespace vulkan