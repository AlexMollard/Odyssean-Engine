#pragma once
#include "vulkan/vulkan.hpp"
namespace vulkan
{
class SemaphoreFence
{
public:
	SemaphoreFence() = default;
	SemaphoreFence(vk::Semaphore semaphore, vk::Fence fence);

	// Getters
	vk::Semaphore getSemaphore() const;
	vk::Fence     getFence() const;

	// Setters
	void setSemaphore(vk::Semaphore semaphore);
	void setFence(vk::Fence fence);

	// Helper functions
	void wait(vk::Device device, uint64_t timeout = UINT64_MAX) const;
	void reset(vk::Device device) const;

private:
	vk::Semaphore m_Semaphore;
	vk::Fence     m_Fence;
};
} // namespace vulkan