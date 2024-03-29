#pragma once
#include "common.h"
#include <vulkan/vulkan.hpp>
namespace VulkanWrapper
{
struct AllocatedBuffer;
enum class QueueType
{
	GRAPHICS,
	PRESENT,
	COMPUTE,
	TRANSFER
};

class DeviceQueue
{
public:
	VulkanWrapper::Buffer CreateBuffer(vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, const void* data, vk::DeviceSize size);
	VulkanWrapper::Buffer CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage);

	vk::Result CopyBuffer(vk::CommandPool& commandPool, vk::Queue queue, vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
	vk::Result FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, uint32_t& memoryTypeIndex);

	DeviceQueue() = default;
	DeviceQueue(vk::Device device);

	// == QUEUE == //
	vk::Queue GetQueue(QueueType queueType) const;
	uint32_t GetQueueIndex(QueueType queueType) const;
	uint32_t GetQueueFamilyIndex(QueueType queueType) const;

	void SetQueue(QueueType queueType, vk::Queue queue);
	void SetQueueIndex(QueueType queueType, uint32_t queueIndex);
	void SetQueueFamilyIndex(QueueType queueType, uint32_t queueFamilyIndex);

	bool IsQueueFamilyIndexSet(QueueType queueType) const;

	void wait()
	{
		m_Device.waitIdle();
	}

	vk::Device m_Device;
	vk::PhysicalDevice m_PhysicalDevice;
	std::vector<const char*> m_DeviceExtensions;

	vk::CommandBuffer BeginSingleTimeCommands(vk::CommandPool& commandPool);

	void EndSingleTimeCommands(vk::CommandPool& commandPool, vk::Queue queue, vk::CommandBuffer commandBuffer);

private:
	vk::Queue m_GraphicsQueue; // Used for pushing geometry to the GPU
	vk::Queue m_PresentQueue;  // Used for presenting the rendered image to the screen
	vk::Queue m_ComputeQueue;  // Used for compute shaders
	vk::Queue m_TransferQueue; // Used for transferring data between buffers

	uint32_t m_GraphicsQueueIndex = UINT32_MAX;
	uint32_t m_PresentQueueIndex  = UINT32_MAX;
	uint32_t m_ComputeQueueIndex  = UINT32_MAX;
	uint32_t m_TransferQueueIndex = UINT32_MAX;

	uint32_t m_GraphicsQueueFamilyIndex = UINT32_MAX;
	uint32_t m_PresentQueueFamilyIndex  = UINT32_MAX;
	uint32_t m_ComputeQueueFamilyIndex  = UINT32_MAX;
	uint32_t m_TransferQueueFamilyIndex = UINT32_MAX;

public:
	void Submit(vk::SubmitInfo submitInfo);
	vk::Result Present(vk::SwapchainKHR& m_Swapchain, uint32_t imageIndex, vk::Semaphore& semaphore);
};
} // namespace VulkanWrapper