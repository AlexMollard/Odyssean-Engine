#include "common.h"

#include "DeviceQueue.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace vulkan
{
vk::Result vulkan::Texture::Create(vulkan::DeviceQueue& devices, vk::CommandPool& commandPool, vk::Queue queue, const void* data, uint32_t width, uint32_t height)
{
	// Create image
	vk::ImageCreateInfo imageInfo = {};
	imageInfo.imageType           = vk::ImageType::e2D;
	imageInfo.format              = vk::Format::eR8G8B8A8Unorm;
	imageInfo.extent.width        = width;
	imageInfo.extent.height       = height;
	imageInfo.extent.depth        = 1;
	imageInfo.mipLevels           = 1;
	imageInfo.arrayLayers         = 1;
	imageInfo.samples             = vk::SampleCountFlagBits::e1;
	imageInfo.tiling              = vk::ImageTiling::eOptimal;
	imageInfo.usage               = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
	image                         = devices.m_Device.createImage(imageInfo);
	if (image == vk::Image()) { return vk::Result::eErrorInitializationFailed; }

	// Allocate memory for the image
	vk::MemoryRequirements memRequirements = devices.m_Device.getImageMemoryRequirements(image);
	uint32_t               memoryTypeIndex;
	vk::Result             result = devices.FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, memoryTypeIndex);
	if (result != vk::Result::eSuccess) { return result; }

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize         = memRequirements.size;
	allocInfo.memoryTypeIndex        = memoryTypeIndex;
	memory                           = devices.m_Device.allocateMemory(allocInfo);
	if (memory == vk::DeviceMemory()) { return vk::Result::eErrorInitializationFailed; }

	devices.m_Device.bindImageMemory(image, memory, 0);

	// Create image view
	vk::ImageViewCreateInfo viewInfo         = {};
	viewInfo.image                           = image;
	viewInfo.viewType                        = vk::ImageViewType::e2D;
	viewInfo.format                          = vk::Format::eR8G8B8A8Unorm;
	viewInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = 1;
	imageView                                = devices.m_Device.createImageView(viewInfo);
	if (imageView == vk::ImageView()) { return vk::Result::eErrorInitializationFailed; }

	// Create sampler
	vk::SamplerCreateInfo samplerInfo   = {};
	samplerInfo.magFilter               = vk::Filter::eLinear;
	samplerInfo.minFilter               = vk::Filter::eLinear;
	samplerInfo.addressModeU            = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV            = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW            = vk::SamplerAddressMode::eRepeat;
	samplerInfo.anisotropyEnable        = VK_FALSE;
	samplerInfo.borderColor             = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable           = VK_FALSE;
	samplerInfo.compareOp               = vk::CompareOp::eAlways;
	samplerInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;

	sampler = devices.m_Device.createSampler(samplerInfo);
	if (sampler == vk::Sampler()) { return vk::Result::eErrorInitializationFailed; }

	// Create staging buffer
	AllocatedBuffer stagingBuffer;
	result = devices.CreateBuffer(vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, data, width * height * 4);
	if (result != vk::Result::eSuccess) { return result; }

	// Copy staging buffer to image
	vk::CommandBuffer         commandBuffer    = devices.BeginSingleTimeCommands(commandPool);
	vk::ImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask                = vk::ImageAspectFlagBits::eColor;
	subresourceRange.baseMipLevel              = 0;
	subresourceRange.levelCount                = 1;
	subresourceRange.baseArrayLayer            = 0;
	subresourceRange.layerCount                = 1;

	vk::ImageMemoryBarrier barrier = {};
	barrier.oldLayout              = vk::ImageLayout::eUndefined;
	barrier.newLayout              = vk::ImageLayout::eTransferDstOptimal;
	barrier.srcQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED;
	barrier.image                  = image;
	barrier.subresourceRange       = subresourceRange;

	barrier.srcAccessMask = vk::AccessFlags();
	barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

	vk::BufferImageCopy region             = {};
	region.bufferOffset                    = 0;
	region.bufferRowLength                 = 0;
	region.bufferImageHeight               = 0;
	region.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount     = 1;
	region.imageOffset                     = vk::Offset3D{ 0, 0, 0 };
	region.imageExtent                     = vk::Extent3D{ width, height, 1 };

	commandBuffer.copyBufferToImage(stagingBuffer.buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

	barrier.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
	barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
	barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

	devices.EndSingleTimeCommands(commandPool, devices.GetQueue(QueueType::GRAPHICS), commandBuffer); // Should probs change queue type to be compute idk

	// Clean up staging resources
	vkDestroyBuffer(devices.m_Device, stagingBuffer.buffer, nullptr);
	vkFreeMemory(devices.m_Device, stagingBuffer.memory, nullptr);

	return vk::Result::eSuccess;
}

vk::Result vulkan::Texture::Load(vulkan::DeviceQueue& devices, vk::CommandPool& commandPool, vk::Queue queue, const char* dir)
{
	int        width, height, channels;
	stbi_uc*   pixels = stbi_load(dir, &width, &height, &channels, STBI_rgb_alpha);
	vk::Result result = Create(devices, commandPool, queue, pixels, width, height);
	stbi_image_free(pixels);
	return result;
}

vk::Result Texture::destroy(vk::Device& device)
{
	if (sampler) { vkDestroySampler(device, sampler, nullptr); }

	vkDestroyImageView(device, imageView, nullptr);
	vkDestroyImage(device, image, nullptr);
	vkFreeMemory(device, memory, nullptr);
	return vk::Result::eSuccess;
}
} // namespace vulkan