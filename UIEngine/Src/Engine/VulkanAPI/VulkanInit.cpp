#include "VulkanInit.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const uint32_t WIDTH  = 1920;
const uint32_t HEIGHT = 1080;

GLFWwindow* create_window_glfw(const char* window_name = "", bool resize = true)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	if (!resize)
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return glfwCreateWindow(1024, 1024, window_name, NULL, NULL);
}

void destroy_window_glfw(GLFWwindow* window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

VkSurfaceKHR create_surface_glfw(VkInstance instance, GLFWwindow* window, VkAllocationCallbacks* allocator = nullptr)
{
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkResult err         = glfwCreateWindowSurface(instance, window, allocator, &surface);
	if (err)
	{
		const char* error_msg;
		int ret = glfwGetError(&error_msg);
		if (ret != 0)
		{
			std::cout << ret << " ";
			if (error_msg != nullptr)
				std::cout << error_msg;
			std::cout << "\n";
		}
		surface = VK_NULL_HANDLE;
	}
	return surface;
}

Init VulkanInit::InitVulkan(int width, int height, std::string_view name)
{
	Init init = {};

	CreateDevice(init, name);

	return init;
}

void VulkanInit::DestroyVulkan(Init& init)
{
	vkb::destroy_device(init.device);
	vkb::destroy_surface(init.instance, init.surface);
	vkb::destroy_instance(init.instance);
	destroy_window_glfw(init.window);
}

uint32_t VulkanInit::FindMemoryType(vk::PhysicalDevice& pysDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties, uint32_t& memoryType)
{
	vk::PhysicalDeviceMemoryProperties memProperties = pysDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			memoryType = i;
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanInit::CreateBuffer(Init& init, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& buffer_memory)
{
	vk::BufferCreateInfo buffer_info = {};
	buffer_info.size                 = size;
	buffer_info.usage                = usage;
	buffer_info.sharingMode          = vk::SharingMode::eExclusive;

	vk::Device vk_device{ init.device.device };
	buffer = vk_device.createBuffer(buffer_info);

	vk::PhysicalDevice vk_pysDevice{ init.device.physical_device };

	vk::MemoryRequirements memRequirements = vk_device.getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo alloc_info = {};
	alloc_info.allocationSize         = memRequirements.size;
	alloc_info.memoryTypeIndex        = FindMemoryType(vk_pysDevice, memRequirements.memoryTypeBits, properties, alloc_info.memoryTypeIndex);

	buffer_memory = vk_device.allocateMemory(alloc_info);

	vk_device.bindBufferMemory(buffer, buffer_memory, 0);
}

void VulkanInit::CopyBuffer(Init& init, RenderData& renderData, vk::Buffer buffer, vk::Buffer buffer2, vk::DeviceSize size)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(init, renderData);

	vk::BufferCopy copyRegion = {};
	copyRegion.size           = size;
	commandBuffer.copyBuffer(buffer, buffer2, copyRegion);
	EndSingleTimeCommands(init, renderData, commandBuffer);
}

void VulkanInit::DestroyBuffer(Init& init, vk::Buffer buffer, vk::DeviceMemory memory)
{
	vk::Device vk_device{ init.device.device };
	vk_device.freeMemory(memory);
	vk_device.destroyBuffer(buffer);
}

void* VulkanInit::LoadImage(Init& init, RenderData& renderData, vulkan::Buffer& stagingBuffer, vulkan::Mesh& mesh, int& width, int& height, int& channels)
{
	vk::Device vk_device{ init.device.device };
	stbi_uc* pixels = stbi_load(mesh.texturePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	// Check if the texture loaded correctly
	if (!pixels)
	{
		std::cout << "Failed to load texture" << std::endl;

		// Instead of returning we will just create a magenta texture
		width        = 1;
		height       = 1;
		channels     = 4;
		pixels       = new stbi_uc[4]{ 255, 0, 255, 255 };
	}

	// Create a staging buffer
	vk::DeviceSize buffer_size = width * height * 4;

	// Create staging buffer
	VulkanInit::CreateBuffer(init, buffer_size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer.buffer, stagingBuffer.memory);

	// Copy the texture data to the staging buffer
	void* data;
	vk_device.mapMemory(stagingBuffer.memory, 0, buffer_size, vk::MemoryMapFlags(), &data);
	memcpy(data, pixels, static_cast<size_t>(buffer_size));
	vk_device.unmapMemory(stagingBuffer.memory);

	// Free the texture data
	stbi_image_free(pixels);

	return data;
}

void VulkanInit::CreateImage(Init& init, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory)
{
	vk::ImageCreateInfo imageInfo = {};
	imageInfo.imageType           = vk::ImageType::e2D;
	imageInfo.extent.width        = width;
	imageInfo.extent.height       = height;
	imageInfo.extent.depth        = 1;
	imageInfo.mipLevels           = 1;
	imageInfo.arrayLayers         = 1;
	imageInfo.format              = format;
	imageInfo.tiling              = tiling;
	imageInfo.initialLayout       = vk::ImageLayout::eUndefined;
	imageInfo.usage               = usage;
	imageInfo.samples             = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode         = vk::SharingMode::eExclusive;

	vk::Device vk_device{ init.device.device };
	image = vk_device.createImage(imageInfo);

	vk::PhysicalDevice vk_pysDevice{ init.device.physical_device };

	vk::MemoryRequirements memRequirements = vk_device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize         = memRequirements.size;
	allocInfo.memoryTypeIndex        = FindMemoryType(vk_pysDevice, memRequirements.memoryTypeBits, properties, allocInfo.memoryTypeIndex);

	imageMemory = vk_device.allocateMemory(allocInfo);

	vk_device.bindImageMemory(image, imageMemory, 0);
}

void VulkanInit::TransitionImageLayout(Init& init, RenderData& renderData, vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(init, renderData);

	vk::ImageMemoryBarrier barrier = {};
	barrier.oldLayout              = oldLayout;
	barrier.newLayout              = newLayout;
	barrier.srcQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED;
	barrier.image                  = image;

	auto HasStencilComponent = [](vk::Format format) -> bool { return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint; };

	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

		if (HasStencilComponent(format))
		{
			barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = 1;

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage      = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, nullptr, nullptr, barrier);

	EndSingleTimeCommands(init, renderData, commandBuffer);
}

void VulkanInit::CopyBufferToImage(Init& init, RenderData& renderData, vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(init, renderData);

	vk::BufferImageCopy region = {};
	region.bufferOffset        = 0;
	region.bufferRowLength     = 0;
	region.bufferImageHeight   = 0;

	region.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount     = 1;

	region.imageOffset = vk::Offset3D{ 0, 0, 0 };
	region.imageExtent = vk::Extent3D{ width, height, 1 };

	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

	EndSingleTimeCommands(init, renderData, commandBuffer);
}

vk::ImageView VulkanInit::CreateImageView(Init& init, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo         = {};
	viewInfo.image                           = image;
	viewInfo.viewType                        = vk::ImageViewType::e2D;
	viewInfo.format                          = format;
	viewInfo.subresourceRange.aspectMask     = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = 1;

	vk::Device vk_device{ init.device.device };

	return vk_device.createImageView(viewInfo);
}

void VulkanInit::DestroyImage(Init& init, vk::Image image, vk::DeviceMemory memory)
{
	vk::Device vk_device{ init.device.device };
	vk_device.destroyImage(image);
	vk_device.freeMemory(memory);
}

void VulkanInit::CreateDevice(Init& init, std::string_view name)
{
	init.window = create_window_glfw(name.data(), true);

	vkb::InstanceBuilder instance_builder;
	auto instance_ret = instance_builder.use_default_debug_messenger().request_validation_layers().build();
	if (!instance_ret)
	{
		std::cout << instance_ret.error().message() << "\n";
		return;
	}
	init.instance = instance_ret.value();

	init.vk_lib.init(init.instance);

	init.surface = create_surface_glfw(init.instance, init.window);

	vkb::PhysicalDeviceSelector phys_device_selector(init.instance);
	auto phys_device_ret = phys_device_selector.set_surface(init.surface).select();
	if (!phys_device_ret)
	{
		std::cout << phys_device_ret.error().message() << "\n";
		return;
	}
	vkb::PhysicalDevice physical_device = phys_device_ret.value();

	vkb::DeviceBuilder device_builder{ physical_device };
	auto device_ret = device_builder.build();
	if (!device_ret)
	{
		std::cout << device_ret.error().message() << "\n";
		return;
	}
	init.device = device_ret.value();
	init.vk_lib.init(init.device);
}

vk::CommandBuffer VulkanInit::BeginSingleTimeCommands(Init& init, RenderData& renderData)
{
	vk::Device vk_device{ init.device.device };

	// Using the command pool, allocate a command buffer
	vk::CommandBufferAllocateInfo alloc_info = {};
	alloc_info.commandPool                   = renderData.command_pool;
	alloc_info.level                         = vk::CommandBufferLevel::ePrimary;
	alloc_info.commandBufferCount            = 1;

	vk::CommandBuffer commandBuffer = vk_device.allocateCommandBuffers(alloc_info)[0];

	// Begin recording the command buffer
	vk::CommandBufferBeginInfo begin_info = {};
	begin_info.flags                      = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	commandBuffer.begin(begin_info);
	return commandBuffer;
}

void VulkanInit::EndSingleTimeCommands(Init& init, RenderData& renderData, vk::CommandBuffer commandBuffer)
{
	vk::Device vk_device{ init.device.device };

	commandBuffer.end();

	// Submit the command buffer to the queue
	vk::SubmitInfo submit_info     = {};
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers    = &commandBuffer;

	vk::Queue queue = vk_device.getQueue(0, 0);
	queue.submit(submit_info, nullptr);
	queue.waitIdle();

	// Free the command buffer back to the pool
	vk_device.freeCommandBuffers(renderData.command_pool, commandBuffer);
}