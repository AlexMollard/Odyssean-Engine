#include "SemaphoreFence.h"
namespace vulkan
{
SemaphoreFence::SemaphoreFence(vk::Semaphore semaphore, vk::Fence fence)
{
	m_Semaphore = semaphore;
	m_Fence     = fence;
}

vk::Semaphore SemaphoreFence::getSemaphore() const
{
	return m_Semaphore;
}

vk::Fence SemaphoreFence::getFence() const
{
	return m_Fence;
}

void SemaphoreFence::setSemaphore(vk::Semaphore semaphore)
{
	m_Semaphore = semaphore;
}

void SemaphoreFence::setFence(vk::Fence fence)
{
	m_Fence = fence;
}

void SemaphoreFence::wait(vk::Device device, uint64_t timeout /*= UINT64_MAX*/) const
{
	if (device.waitForFences(m_Fence, VK_TRUE, timeout) != vk::Result::eSuccess) { throw std::runtime_error("Failed to wait for fence"); }
}

void SemaphoreFence::reset(vk::Device device) const
{
	device.resetFences(m_Fence);
}
} // namespace vulkan