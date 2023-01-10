#include "SwapchainInfo.h"
namespace vulkan
{
SwapchainInfo::SwapchainInfo(vk::SwapchainKHR swapchain, vk::Extent2D extent, vk::Format format, vk::ImageUsageFlags imageUsage, vk::SurfaceTransformFlagBitsKHR preTransform, vk::PresentModeKHR presentMode,
							 vk::ArrayProxy<const vk::Image> images)
{
	m_Swapchain    = swapchain;
	m_Extent       = extent;
	m_Format       = format;
	m_ImageUsage   = imageUsage;
	m_PreTransform = preTransform;
	m_PresentMode  = presentMode;
	m_Images       = images;
}

} // namespace vulkan