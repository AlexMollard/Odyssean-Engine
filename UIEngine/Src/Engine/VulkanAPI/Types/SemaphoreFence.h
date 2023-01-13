#pragma once
#include "vulkan/vulkan.hpp"
namespace vulkan
{
class SemaphoreFence
{
public:
	SemaphoreFence() = default;

	vk::Fence              m_InFlight;
	std::vector<vk::Fence> m_ImagesInFlight;

	vk::Semaphore m_ImageAvailable;
	vk::Semaphore m_RenderFinished;

	// Helper functions

	// Returns true if the in flight fence is signaled
	vk::Result WaitForFence(vk::Device& device)
	{
		return device.waitForFences(m_InFlight, VK_TRUE, UINT64_MAX);
	}
	vk::Result WaitForFence(vk::Device& device, vk::Fence& fence)
	{
		return device.waitForFences(fence, VK_TRUE, UINT64_MAX);
	}

	// Resets the inflight fence
	void ResetFence(vk::Device& device)
	{
		device.resetFences(m_InFlight);
	}
	
	void ResetFence(vk::Device& device, std::vector<vk::Fence>& fences)
	{
		device.resetFences(fences);
	}

	vk::SubmitInfo GetSubmitInfo(const vk::CommandBuffer& m_CommandBuffer, uint32_t imageIndex);

	void destroy(vk::Device& device);
};
} // namespace vulkan