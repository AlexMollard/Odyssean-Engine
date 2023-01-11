#pragma once
#include "vulkan/vulkan.hpp"
namespace vulkan
{
struct SwapchainInfo
{
	SwapchainInfo() = default;
	SwapchainInfo(vk::SwapchainKHR swapchain, vk::Extent2D extent, vk::Format format, vk::ImageUsageFlags imageUsage, vk::SurfaceTransformFlagBitsKHR preTransform, vk::PresentModeKHR presentMode,
				  std::vector<vk::Image> images);

	vk::SwapchainKHR                m_Swapchain;
	vk::Extent2D                    m_Extent;
	vk::Format                      m_Format;
	vk::ImageUsageFlags             m_ImageUsage;
	vk::SurfaceTransformFlagBitsKHR m_PreTransform;
	vk::PresentModeKHR              m_PresentMode;
	std::vector<vk::Image>          m_Images;
	std::vector<vk::ImageView>      m_ImageViews;
	uint32_t                        m_ImageCount;
};
} // namespace vulkan