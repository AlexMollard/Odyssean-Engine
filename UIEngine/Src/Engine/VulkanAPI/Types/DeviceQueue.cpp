#include "DeviceQueue.h"

#include "common.h"
namespace VulkanWrapper
{
DeviceQueue::DeviceQueue(vk::Device device)
{
	m_Device = device;
}

vk::Queue DeviceQueue::GetQueue(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: return m_GraphicsQueue;
	case QueueType::PRESENT: return m_PresentQueue;
	case QueueType::COMPUTE: return m_ComputeQueue;
	case QueueType::TRANSFER: return m_TransferQueue;
	default: return nullptr;
	}
}

uint32_t DeviceQueue::GetQueueIndex(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: return m_GraphicsQueueIndex;
	case QueueType::PRESENT: return m_PresentQueueIndex;
	case QueueType::COMPUTE: return m_ComputeQueueIndex;
	case QueueType::TRANSFER: return m_TransferQueueIndex;
	default: return 0;
	}
}

uint32_t DeviceQueue::GetQueueFamilyIndex(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: return m_GraphicsQueueFamilyIndex;
	case QueueType::PRESENT: return m_PresentQueueFamilyIndex;
	case QueueType::COMPUTE: return m_ComputeQueueFamilyIndex;
	case QueueType::TRANSFER: return m_TransferQueueFamilyIndex;
	default: return 0;
	}
}

void DeviceQueue::SetQueue(QueueType queueType, vk::Queue queue)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: m_GraphicsQueue = queue; break;
	case QueueType::PRESENT: m_PresentQueue = queue; break;
	case QueueType::COMPUTE: m_ComputeQueue = queue; break;
	case QueueType::TRANSFER: m_TransferQueue = queue; break;
	default: break;
	}
}

void DeviceQueue::SetQueueIndex(QueueType queueType, uint32_t queueIndex)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: m_GraphicsQueueIndex = queueIndex; break;
	case QueueType::PRESENT: m_PresentQueueIndex = queueIndex; break;
	case QueueType::COMPUTE: m_ComputeQueueIndex = queueIndex; break;
	case QueueType::TRANSFER: m_TransferQueueIndex = queueIndex; break;
	default: break;
	}
}

void DeviceQueue::SetQueueFamilyIndex(QueueType queueType, uint32_t queueFamilyIndex)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: m_GraphicsQueueFamilyIndex = queueFamilyIndex; break;
	case QueueType::PRESENT: m_PresentQueueFamilyIndex = queueFamilyIndex; break;
	case QueueType::COMPUTE: m_ComputeQueueFamilyIndex = queueFamilyIndex; break;
	case QueueType::TRANSFER: m_TransferQueueFamilyIndex = queueFamilyIndex; break;
	default: break;
	}
}

bool DeviceQueue::IsQueueFamilyIndexSet(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: return m_GraphicsQueueFamilyIndex != UINT32_MAX;
	case QueueType::PRESENT: return m_PresentQueueFamilyIndex != UINT32_MAX;
	case QueueType::COMPUTE: return m_ComputeQueueFamilyIndex != UINT32_MAX;
	case QueueType::TRANSFER: return m_TransferQueueFamilyIndex != UINT32_MAX;
	default: return false;
	}
}

vk::CommandBuffer DeviceQueue::BeginSingleTimeCommands(vk::CommandPool& commandPool)
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(commandPool);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandBufferCount(1);

	vk::CommandBuffer commandBuffer = m_Device.allocateCommandBuffers(allocInfo)[0];

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void DeviceQueue::EndSingleTimeCommands(vk::CommandPool& commandPool, vk::Queue queue, vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&commandBuffer);

	queue.submit(submitInfo, nullptr);
	queue.waitIdle();

	m_Device.freeCommandBuffers(commandPool, commandBuffer);
}

void DeviceQueue::Submit(vk::SubmitInfo submitInfo)
{
	m_GraphicsQueue.submit(submitInfo, nullptr);
}

vk::Result DeviceQueue::Present(vk::SwapchainKHR& m_Swapchain, uint32_t imageIndex, vk::Semaphore& semaphore)
{
	vk::PresentInfoKHR presentInfo;
	presentInfo.setWaitSemaphoreCount(1);
	presentInfo.setPWaitSemaphores(&semaphore);
	presentInfo.setSwapchainCount(1);
	presentInfo.setPSwapchains(&m_Swapchain);
	presentInfo.setPImageIndices(&imageIndex);
	return m_PresentQueue.presentKHR(presentInfo);
}

VulkanWrapper::Buffer DeviceQueue::CreateBuffer(vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, const void* data, vk::DeviceSize size)
{
	VulkanWrapper::Buffer buffer;

	// Create the buffer
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size                 = size;
	bufferInfo.usage                = usage;
	buffer.buffer                   = m_Device.createBuffer(bufferInfo);

	// Allocate memory for the buffer
	vk::MemoryRequirements memRequirements = m_Device.getBufferMemoryRequirements(buffer.buffer);
	uint32_t               memoryTypeIndex = 0;
	VK_CHECK_RESULT(FindMemoryType(memRequirements.memoryTypeBits, properties, memoryTypeIndex));

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize         = memRequirements.size;
	allocInfo.memoryTypeIndex        = memoryTypeIndex;
	buffer.memory                    = m_Device.allocateMemory(allocInfo);

	// Bind the buffer memory
	m_Device.bindBufferMemory(buffer.buffer, buffer.memory, 0);

	// Copy data to the buffer
	if (data != nullptr)
	{
		void* mapped = m_Device.mapMemory(buffer.memory, 0, size, {});
		memcpy(mapped, data, size);
		m_Device.unmapMemory(buffer.memory);
	}

	return buffer;
}

VulkanWrapper::Buffer DeviceQueue::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size                 = size;
	bufferInfo.usage                = usage;
	return { m_Device.createBuffer(bufferInfo) };
}

vk::Result DeviceQueue::CopyBuffer(vk::CommandPool& commandPool, vk::Queue queue, vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	// Allocate a command buffer
	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool                   = commandPool;
	allocInfo.level                         = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount            = 1;
	vk::CommandBuffer commandBuffer         = m_Device.allocateCommandBuffers(allocInfo)[0];

	// Record copy command
	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags                      = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBuffer.begin(beginInfo);

	if (!commandBuffer) { VK_CHECK_RESULT(vk::Result::eErrorInitializationFailed); }

	vk::BufferCopy copyRegion = {};
	copyRegion.size           = size;
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
	commandBuffer.end();

	// Submit and wait for the copy to complete
	vk::SubmitInfo submitInfo     = {};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;
	VK_CHECK_RESULT(queue.submit(1, &submitInfo, nullptr));
	queue.waitIdle();

	// Free the command buffer
	m_Device.freeCommandBuffers(commandPool, 1, &commandBuffer);

	return vk::Result::eSuccess;
}

vk::Result DeviceQueue::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, uint32_t& memoryTypeIndex)
{
	// Get the memory properties of the physical device
	vk::PhysicalDeviceMemoryProperties memProperties = m_PhysicalDevice.getMemoryProperties();

	// Iterate over the memory types and find one that matches the type filter and properties
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			memoryTypeIndex = i;
			return vk::Result::eSuccess;
		}
	}

	// If no memory type was found, return an error
	return vk::Result::eErrorMemoryMapFailed;
}
} // namespace VulkanWrapper