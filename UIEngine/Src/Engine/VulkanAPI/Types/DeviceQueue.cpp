#include "DeviceQueue.h"
namespace vulkan
{

DeviceQueue::DeviceQueue(vk::Device device)
{
	m_Device = device;
}

vk::Queue DeviceQueue::GetQueue(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS:
		return m_GraphicsQueue;
	case QueueType::PRESENT:
		return m_PresentQueue;
	case QueueType::COMPUTE:
		return m_ComputeQueue;
	case QueueType::TRANSFER:
		return m_TransferQueue;
	default:
		return nullptr;
	}
}

uint32_t DeviceQueue::GetQueueIndex(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS:
		return m_GraphicsQueueIndex;
	case QueueType::PRESENT:
		return m_PresentQueueIndex;
	case QueueType::COMPUTE:
		return m_ComputeQueueIndex;
	case QueueType::TRANSFER:
		return m_TransferQueueIndex;
	default:
		return 0;
	}
}

uint32_t DeviceQueue::GetQueueFamilyIndex(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS:
		return m_GraphicsQueueFamilyIndex;
	case QueueType::PRESENT:
		return m_PresentQueueFamilyIndex;
	case QueueType::COMPUTE:
		return m_ComputeQueueFamilyIndex;
	case QueueType::TRANSFER:
		return m_TransferQueueFamilyIndex;
	default:
		return 0;
	}
}

void DeviceQueue::SetQueue(QueueType queueType, vk::Queue queue)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS:
		m_GraphicsQueue = queue;
		break;
	case QueueType::PRESENT:
		m_PresentQueue = queue;
		break;
	case QueueType::COMPUTE:
		m_ComputeQueue = queue;
		break;
	case QueueType::TRANSFER:
		m_TransferQueue = queue;
		break;
	default:
		break;
	}
}

void DeviceQueue::SetQueueIndex(QueueType queueType, uint32_t queueIndex)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS:
		m_GraphicsQueueIndex = queueIndex;
		break;
	case QueueType::PRESENT:
		m_PresentQueueIndex = queueIndex;
		break;
	case QueueType::COMPUTE:
		m_ComputeQueueIndex = queueIndex;
		break;
	case QueueType::TRANSFER:
		m_TransferQueueIndex = queueIndex;
		break;
	default:
		break;
	}
}

void DeviceQueue::SetQueueFamilyIndex(QueueType queueType, uint32_t queueFamilyIndex)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS:
		m_GraphicsQueueFamilyIndex = queueFamilyIndex;
		break;
	case QueueType::PRESENT:
		m_PresentQueueFamilyIndex = queueFamilyIndex;
		break;
	case QueueType::COMPUTE:
		m_ComputeQueueFamilyIndex = queueFamilyIndex;
		break;
	case QueueType::TRANSFER:
		m_TransferQueueFamilyIndex = queueFamilyIndex;
		break;
	default:
		break;
	}
}

bool DeviceQueue::IsQueueFamilyIndexSet(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS:
		return m_GraphicsQueueFamilyIndex != UINT32_MAX;
	case QueueType::PRESENT:
		return m_PresentQueueFamilyIndex != UINT32_MAX;
	case QueueType::COMPUTE:
		return m_ComputeQueueFamilyIndex != UINT32_MAX;
	case QueueType::TRANSFER:
		return m_TransferQueueFamilyIndex != UINT32_MAX;
	default:
		return false;
	}
}

} // namespace vulkan