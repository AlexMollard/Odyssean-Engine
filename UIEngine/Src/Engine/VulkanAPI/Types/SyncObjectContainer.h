#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanWrapper
{
class SyncObjectContainer
{
public:
	// constructor with number of frames in flight as an argument
	SyncObjectContainer() = default;
	~SyncObjectContainer();

	// initialize the sync objects
	vk::Result init(vk::Device* device, uint32_t framesInFlight);

	// cleanup the sync objects
	void cleanup();

	// reset the fences
	void resetFences();

	// wait for the fences
	vk::Result waitForFences();

	// get the image available semaphore
	vk::Semaphore& getImageAvailableSemaphore();

	// get the render finished semaphore
	vk::Semaphore& getRenderFinishedSemaphore();

	// get the in flight fence
	vk::Fence& getInFlightFence();

	// get the image in flight fence at the given index
	vk::Fence& getImageInFlightFence(size_t index);

	// get submit info
	vk::SubmitInfo getSubmitInfo(const vk::CommandBuffer& commandBuffer);

	// Gets the current frame
	uint32_t getCurrentFrame() const
	{
		return m_CurrentFrame;
	}

	// Sets the current frame
	void setCurrentFrame(uint32_t val)
	{
		m_CurrentFrame = val;
	}

private:
	vk::Device*            m_Device = nullptr;
	uint32_t               m_FramesInFlight;
	uint32_t               m_CurrentFrame;
	vk::Semaphore          m_ImageAvailableSemaphore;
	vk::Semaphore          m_RenderFinishedSemaphore;
	std::vector<vk::Fence> m_InFlightFences;
	std::vector<vk::Fence> m_ImagesInFlight;
};
} // namespace VulkanWrapper
