#include "SyncObjectContainer.h"

vulkan::SyncObjectContainer::~SyncObjectContainer()
{
}

vk::Result vulkan::SyncObjectContainer::init(vk::Device* device, uint32_t framesInFlight)
{
	m_Device = device;
	m_FramesInFlight = framesInFlight;

	vk::SemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.setFlags(vk::SemaphoreCreateFlags());

	vk::FenceCreateInfo fenceInfo = {};
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

	// Create the semaphores
	vk::Result result = m_Device->createSemaphore(&semaphoreInfo, nullptr, &m_ImageAvailableSemaphore);
	if (result != vk::Result::eSuccess) { return result; }
	result = m_Device->createSemaphore(&semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
	if (result != vk::Result::eSuccess) { return result; }

	// Create the fences
	m_InFlightFences.resize(m_FramesInFlight);
	for (auto& fence : m_InFlightFences)
	{
		result = m_Device->createFence(&fenceInfo, nullptr, &fence);
		if (result != vk::Result::eSuccess) { return result; }
	}

	m_ImagesInFlight.resize(m_FramesInFlight);
	for (auto& fence : m_ImagesInFlight)
	{
		result = m_Device->createFence(&fenceInfo, nullptr, &fence);
		if (result != vk::Result::eSuccess) { return result; }
	}

	return result;
}

void vulkan::SyncObjectContainer::cleanup()
{
	// Destroy the semaphores
	m_Device->destroySemaphore(m_ImageAvailableSemaphore);
	m_Device->destroySemaphore(m_RenderFinishedSemaphore);

	// Destroy the fences
	for (auto& fence : m_InFlightFences) { m_Device->destroyFence(fence); }
	for (auto& fence : m_ImagesInFlight) { m_Device->destroyFence(fence); }

	m_InFlightFences.clear();
	m_ImagesInFlight.clear();
}

void vulkan::SyncObjectContainer::resetFences()
{
	m_Device->resetFences(m_InFlightFences[m_CurrentFrame]);
}

vk::Result vulkan::SyncObjectContainer::waitForFences()
{
	return m_Device->waitForFences(m_InFlightFences, VK_TRUE, UINT64_MAX);
}

vk::Semaphore& vulkan::SyncObjectContainer::getImageAvailableSemaphore()
{
	return m_ImageAvailableSemaphore;
}

vk::Semaphore& vulkan::SyncObjectContainer::getRenderFinishedSemaphore()
{
	return m_RenderFinishedSemaphore;
}

vk::Fence& vulkan::SyncObjectContainer::getInFlightFence()
{
	return m_InFlightFences[m_CurrentFrame];
}

vk::Fence& vulkan::SyncObjectContainer::getImageInFlightFence(size_t index)
{
	return m_ImagesInFlight[index];
}

vk::SubmitInfo vulkan::SyncObjectContainer::getSubmitInfo(const vk::CommandBuffer& commandBuffer)
{
	vk::SubmitInfo submitInfo = {};
	submitInfo.setWaitSemaphoreCount(1);
	submitInfo.setPWaitSemaphores(&m_ImageAvailableSemaphore);
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&commandBuffer);
	submitInfo.setSignalSemaphoreCount(1);
	submitInfo.setPSignalSemaphores(&m_RenderFinishedSemaphore);
	return submitInfo;
}