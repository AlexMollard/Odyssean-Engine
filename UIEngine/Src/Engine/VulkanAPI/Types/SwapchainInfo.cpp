#include "SwapchainInfo.h"
namespace VulkanWrapper
{
SwapchainInfo::SwapchainInfo(vk::SwapchainKHR swapchain, vk::Extent2D extent, vk::Format format, vk::ImageUsageFlags imageUsage, vk::SurfaceTransformFlagBitsKHR preTransform, vk::PresentModeKHR presentMode, std::vector<vk::Image> images)
{
	m_Swapchain    = swapchain;
	m_Extent       = extent;
	m_Format       = format;
	m_ImageUsage   = imageUsage;
	m_PreTransform = preTransform;
	m_PresentMode  = presentMode;
	m_Images       = images;
}

vk::Result SwapchainInfo::GetNextImage(vk::Device device, vk::Semaphore& m_ImageAvailable)
{
	return device.acquireNextImageKHR(m_Swapchain, UINT64_MAX, m_ImageAvailable, nullptr, &m_CurrentImage);
}

uint32_t SwapchainInfo::getCurrentFrameIndex()
{
	return m_CurrentImage;
}
} // namespace VulkanWrapper