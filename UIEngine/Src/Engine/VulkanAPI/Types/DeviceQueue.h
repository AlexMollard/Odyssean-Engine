#pragma once
#include "vulkan/vulkan.hpp"
namespace vulkan
{

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
	DeviceQueue() = default;
	DeviceQueue(vk::Device device);

	// == QUEUE == //
	vk::Queue GetQueue(QueueType queueType) const;
	uint32_t  GetQueueIndex(QueueType queueType) const;
	uint32_t  GetQueueFamilyIndex(QueueType queueType) const;

	void SetQueue(QueueType queueType, vk::Queue queue);
	void SetQueueIndex(QueueType queueType, uint32_t queueIndex);
	void SetQueueFamilyIndex(QueueType queueType, uint32_t queueFamilyIndex);

	bool IsQueueFamilyIndexSet(QueueType queueType) const;
	
	void wait()
	{
		m_Device.waitIdle();
	}

	vk::Device m_Device;
	vk::PhysicalDevice       m_PhysicalDevice;
	std::vector<const char*> m_DeviceExtensions;

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
} // namespace vulkan