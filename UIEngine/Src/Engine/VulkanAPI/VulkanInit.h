#pragma once
#include "VkMesh.h"
#include "common.h"
#include "vulkan/vulkan.hpp"

struct Init
{
	GLFWwindow*    window;
	VulkanLibrary  vk_lib;
	vkb::Instance  instance;
	vk::SurfaceKHR surface;
	vkb::Device    device;
	vkb::Swapchain swapchain;
	//convenience
	VulkanLibrary* operator->()
	{
		return &vk_lib;
	}
};

struct ImguiData
{
	// We will be using the built-in Imgui pipeline
	vk::DescriptorPool      descriptor_pool;
	vk::DescriptorSet       descriptor_set;
	vk::DescriptorSetLayout descriptor_set_layout;

	vk::CommandPool                command_pool;
	std::vector<vk::CommandBuffer> command_buffers;

	vk::RenderPass render_pass;
};

struct RenderData
{
	vk::Queue graphics_queue;
	vk::Queue present_queue;

	std::vector<VkImage>       swapchain_images;
	std::vector<VkImageView>   swapchain_image_views;
	std::vector<VkFramebuffer> framebuffers;

	vk::RenderPass     render_pass;
	vk::PipelineLayout pipeline_layout;
	vk::Pipeline       graphics_pipeline;

	vk::DescriptorSetLayout descriptor_set_layout;

	vk::CommandPool                command_pool;
	std::vector<vk::CommandBuffer> command_buffers;

	// Descrpitor Sets
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<vk::Semaphore> available_semaphores;
	std::vector<vk::Semaphore> finished_semaphore;
	std::vector<vk::Fence>     in_flight_fences;
	std::vector<vk::Fence>     image_in_flight;
	size_t                     current_frame = 0;

	// Imgui
	ImguiData imgui_data;

	// Descriptor Pool for imgui
	vk::DescriptorPool imgui_descriptor_pool;
	
	bool has_imgui_commands = false;
};

class VulkanInit
{
public:
	static Init     InitVulkan(int width, int height, std::string_view name);
	static void     DestroyVulkan(Init& init);
	static uint32_t FindMemoryType(vk::PhysicalDevice& pysDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties, uint32_t& memoryType);

	static void CreateBuffer(Init& init, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& buffer_memory);
	static void CopyBuffer(Init& init, RenderData& renderData, vk::Buffer buffer, vk::Buffer buffer2, vk::DeviceSize size);
	static void DestroyBuffer(Init& init, vk::Buffer buffer, vk::DeviceMemory memory);

	// Image Stuff
	static void*         LoadImage(Init& init, RenderData& renderData, vulkan::Buffer& stagingBuffer, vulkan::Mesh& mesh, int& width, int& height, int& channels);
	static void          CreateImage(Init& init, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
	static void          TransitionImageLayout(Init& init, RenderData& renderData, vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	static void          CopyBufferToImage(Init& init, RenderData& renderData, vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
	static vk::ImageView CreateImageView(Init& init, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);
	static void          DestroyImage(Init& init, vk::Image image, vk::DeviceMemory memory);

	static void              EndSingleTimeCommands(Init& init, RenderData& renderData, vk::CommandBuffer commandBuffer);
	static vk::CommandBuffer BeginSingleTimeCommands(Init& init, RenderData& renderData);

private:
	static void CreateDevice(Init& init, std::string_view name);
};
