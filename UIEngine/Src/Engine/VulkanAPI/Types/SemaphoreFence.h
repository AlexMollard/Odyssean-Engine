#pragma once
#include "vulkan/vulkan.hpp"
namespace vulkan
{
class SemaphoreFence
{
public:
	SemaphoreFence() = default;

	vk::Fence m_InFlight;
	std::vector<vk::Fence> m_ImagesInFlight;

	vk::Semaphore m_ImageAvailable;
	vk::Semaphore m_RenderFinished;
};
} // namespace vulkan