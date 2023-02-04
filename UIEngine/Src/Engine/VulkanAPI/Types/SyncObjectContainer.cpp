#include "pch.h"
#include "SyncObjectContainer.h"

#include "common.h"

VulkanWrapper::SyncObjectContainer::~SyncObjectContainer()
{}

vk::Result VulkanWrapper::SyncObjectContainer::init(vk::Device* device, uint32_t framesInFlight)
{
	m_Device         = device;
	m_FramesInFlight = framesInFlight;

	vk::SemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.setFlags(vk::SemaphoreCreateFlags());

	vk::FenceCreateInfo fenceInfo = {};
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

	// Create the semaphores
	VK_CHECK_RESULT(m_Device->createSemaphore(&semaphoreInfo, nullptr, &m_ImageAvailableSemaphore));
	VK_CHECK_RESULT(m_Device->createSemaphore(&semaphoreInfo, nullptr, &m_RenderFinishedSemaphore));

	// Create the fences
	m_InFlightFences.resize(m_FramesInFlight);
	for (auto& fence : m_InFlightFences) { VK_CHECK_RESULT(m_Device->createFence(&fenceInfo, nullptr, &fence)); }

	m_ImagesInFlight.resize(m_FramesInFlight);
	for (auto& fence : m_ImagesInFlight) { VK_CHECK_RESULT(m_Device->createFence(&fenceInfo, nullptr, &fence)); }

	return vk::Result::eSuccess;
}

void VulkanWrapper::SyncObjectContainer::cleanup()
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

void VulkanWrapper::SyncObjectContainer::resetFences()
{
	m_Device->resetFences(m_InFlightFences[m_CurrentFrame]);
}

vk::Result VulkanWrapper::SyncObjectContainer::waitForFences()
{
	return m_Device->waitForFences(m_InFlightFences, VK_TRUE, UINT64_MAX);
}

vk::Semaphore& VulkanWrapper::SyncObjectContainer::getImageAvailableSemaphore()
{
	return m_ImageAvailableSemaphore;
}

vk::Semaphore& VulkanWrapper::SyncObjectContainer::getRenderFinishedSemaphore()
{
	return m_RenderFinishedSemaphore;
}

vk::Fence& VulkanWrapper::SyncObjectContainer::getInFlightFence()
{
	return m_InFlightFences[m_CurrentFrame];
}

vk::Fence& VulkanWrapper::SyncObjectContainer::getImageInFlightFence(size_t index)
{
	return m_ImagesInFlight[index];
}

vk::SubmitInfo VulkanWrapper::SyncObjectContainer::getSubmitInfo(const vk::CommandBuffer& commandBuffer)
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