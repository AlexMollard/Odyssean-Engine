#include "SemaphoreFence.h"
namespace vulkan
{

vk::SubmitInfo SemaphoreFence::GetSubmitInfo(const vk::CommandBuffer& m_CommandBuffer, uint32_t imageIndex)
{
	vk::SubmitInfo submitInfo;

	if (m_CommandBuffer)
	{
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers    = &m_CommandBuffer;
	}
	else 
	{
		submitInfo.commandBufferCount = 0;
		submitInfo.pCommandBuffers    = nullptr;	
	}

	submitInfo.waitSemaphoreCount   = 1;
	submitInfo.pWaitSemaphores      = &m_ImageAvailable;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = &m_RenderFinished;

	return submitInfo;
}

void SemaphoreFence::destroy(vk::Device& device)
{
	if (m_ImageAvailable) { device.destroySemaphore(m_ImageAvailable); }
	if (m_RenderFinished) { device.destroySemaphore(m_RenderFinished); }

	static int test = 0;

	for (auto& fence : m_ImagesInFlight)
	{
		if (fence)
		{
			test++;
			vk::Result result = device.getFenceStatus(fence);
			if (result == vk::Result::eSuccess) { device.destroyFence(fence); }
		}
	}

	// Make sure all fences are destroyed

	// Reset all the handles
	m_ImageAvailable = nullptr;
	m_RenderFinished = nullptr;
	m_InFlight       = nullptr;
	m_ImagesInFlight.clear();
}

} // namespace vulkan