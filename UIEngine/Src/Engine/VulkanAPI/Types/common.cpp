#include "common.h"

#include "DeviceQueue.h"

#define STB_IMAGE_IMPLEMENTATION
#include "VkContainer.h"
#include "stb_image.h"

namespace VulkanWrapper
{
void VulkanWrapper::Texture::Create(VulkanWrapper::VkContainer& api, vk::Queue queue, const void* data, uint32_t width, uint32_t height)
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
	image                         = api.deviceQueue.m_Device.createImage(imageInfo);

	if (image == vk::Image()) VK_CHECK_RESULT(vk::Result::eErrorInitializationFailed);

	// Allocate memory for the image
	vk::MemoryRequirements memRequirements = api.deviceQueue.m_Device.getImageMemoryRequirements(image);
	uint32_t               memoryTypeIndex;
	vk::Result             result = api.deviceQueue.FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, memoryTypeIndex);
	VK_CHECK_RESULT(result);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize         = memRequirements.size;
	allocInfo.memoryTypeIndex        = memoryTypeIndex;
	memory                           = api.deviceQueue.m_Device.allocateMemory(allocInfo);
	if (memory == vk::DeviceMemory()) VK_CHECK_RESULT(vk::Result::eErrorInitializationFailed);

	api.deviceQueue.m_Device.bindImageMemory(image, memory, 0);

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
	imageView                                = api.deviceQueue.m_Device.createImageView(viewInfo);
	if (imageView == vk::ImageView()) VK_CHECK_RESULT(vk::Result::eErrorInitializationFailed);

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

	sampler = api.deviceQueue.m_Device.createSampler(samplerInfo);
	if (sampler == vk::Sampler()) VK_CHECK_RESULT(vk::Result::eErrorInitializationFailed);

	// Create staging buffer
	VulkanWrapper::Buffer stagingBuffer = api.deviceQueue.CreateBuffer(vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, data, width * height * 4);

	// Copy staging buffer to image
	vk::CommandBuffer         commandBuffer    = api.deviceQueue.BeginSingleTimeCommands(api.commandPool);
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

	api.deviceQueue.EndSingleTimeCommands(api.commandPool, api.deviceQueue.GetQueue(QueueType::GRAPHICS), commandBuffer); // Should probs change queue type to be compute idk

	// Clean up staging resources
	vkDestroyBuffer(api.deviceQueue.m_Device, stagingBuffer.buffer, nullptr);
	vkFreeMemory(api.deviceQueue.m_Device, stagingBuffer.memory, nullptr);

	descriptorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	descriptorImageInfo.imageView   = imageView;
	descriptorImageInfo.sampler     = sampler;
}

VulkanWrapper::Texture* VulkanWrapper::Texture::Load(VulkanWrapper::VkContainer& api, vk::Queue queue, const char* dir)
{
	// Check if the std::map texture cache on api already contains this texture
	if (api.textureCache.find(dir) != api.textureCache.end()) { return api.textureCache[dir]; }

	int width, height, channels;

	stbi_uc* pixels = stbi_load(dir, &width, &height, &channels, STBI_rgb_alpha);

	auto texture = new Texture();
	texture->Create(api, queue, pixels, width, height);

	stbi_image_free(pixels);

	api.textureCache[dir] = texture;

	return texture;
}

vk::Result Texture::destroy(vk::Device& device)
{
	if (sampler) { vkDestroySampler(device, sampler, nullptr); }

	vkDestroyImageView(device, imageView, nullptr);
	vkDestroyImage(device, image, nullptr);
	vkFreeMemory(device, memory, nullptr);
	return vk::Result::eSuccess;
}

const vk::DescriptorImageInfo& Texture::GetDescriptorImageInfo() const
{
	return descriptorImageInfo;
}

vk::MappedMemoryRange Buffer::Update(vk::Device& device, const void* data, size_t size)
{
	void* mapped;
	vkMapMemory(device, memory, 0, size, 0, &mapped);
	memcpy(mapped, data, size);
	vkUnmapMemory(device, memory);

	vk::MappedMemoryRange range = {};
	range.memory                = memory;
	range.size                  = VK_WHOLE_SIZE; // Change here

	return range;
}
} // namespace VulkanWrapper