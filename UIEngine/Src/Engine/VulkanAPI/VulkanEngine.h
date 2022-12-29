#pragma once
#include "VulkanRenderer.h"

#include "Engine/BaseEngine.h"
#include "common.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

struct Init
{
	GLFWwindow* window;
	VulkanLibrary vk_lib;
	vkb::Instance instance;
	VkSurfaceKHR surface;
	vkb::Device device;
	vkb::Swapchain swapchain;
	//convenience
	VulkanLibrary* operator->() { return &vk_lib; }
};

struct RenderData
{
	VkQueue graphics_queue;
	VkQueue present_queue;

	std::vector<VkImage> swapchain_images;
	std::vector<VkImageView> swapchain_image_views;
	std::vector<VkFramebuffer> framebuffers;

	VkRenderPass render_pass;
	VkPipelineLayout pipeline_layout;
	VkPipeline graphics_pipeline;

	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> command_buffers;

	std::vector<VkSemaphore> available_semaphores;
	std::vector<VkSemaphore> finished_semaphore;
	std::vector<VkFence> in_flight_fences;
	std::vector<VkFence> image_in_flight;
	size_t current_frame = 0;
};

struct GLFWwindow;
class VulkanEngine : BaseEngine
{
public:
	VulkanEngine() = default;
	~VulkanEngine();

	void Initialize(const char* windowName, int width, int height);

	// Returns DT
	float Update();
	void Render();

	bool GetClose() const { return m_close; }
	void* GetWindow() const { return m_Window; }

	const VulkanRenderer* GetRenderer() const { return &m_Renderer; }

private:

	GLFWwindow* m_Window;
	bool m_close = false;

	Init* init = nullptr;
	RenderData* render_data = nullptr;

	VulkanRenderer m_Renderer;
};
