#pragma once
#include "vulkan/vulkan.hpp"
namespace vulkan
{
class SwapchainInfo
{
public:
	SwapchainInfo() = default;
	SwapchainInfo(vk::SwapchainKHR swapchain, vk::Extent2D extent, vk::Format format, vk::ImageUsageFlags imageUsage, vk::SurfaceTransformFlagBitsKHR preTransform, vk::PresentModeKHR presentMode, vk::ArrayProxy<const vk::Image> images);

	// Getters
	vk::SwapchainKHR getSwapchain() const
	{
		return m_Swapchain;
	};
	vk::Extent2D getExtent() const
	{
		return m_Extent;
	};
	vk::Format getFormat() const
	{
		return m_Format;
	};
	vk::ImageUsageFlags getImageUsage() const
	{
		return m_ImageUsage;
	};
	vk::SurfaceTransformFlagBitsKHR getPreTransform() const
	{
		return m_PreTransform;
	};
	vk::PresentModeKHR getPresentMode() const
	{
		return m_PresentMode;
	};
	vk::ArrayProxy<const vk::Image> getImages() const
	{
		return m_Images;
	};
	uint32_t getImageCount() const
	{
		return m_ImageCount;
	};

	// Setters
	void setSwapchain(vk::SwapchainKHR swapchain)
	{
		m_Swapchain = swapchain;
	};
	void setExtent(vk::Extent2D extent)
	{
		m_Extent = extent;
	};
	void setFormat(vk::Format format)
	{
		m_Format = format;
	};
	void setImageUsage(vk::ImageUsageFlags imageUsage)
	{
		m_ImageUsage = imageUsage;
	};
	void setPreTransform(vk::SurfaceTransformFlagBitsKHR preTransform)
	{
		m_PreTransform = preTransform;
	};
	void setPresentMode(vk::PresentModeKHR presentMode)
	{
		m_PresentMode = presentMode;
	};
	void setImages(vk::ArrayProxy<const vk::Image> images)
	{
		m_Images = images;
	};
	void setImageCount(uint32_t imageCount)
	{
		m_ImageCount = imageCount;
	};

private:
	vk::SwapchainKHR                m_Swapchain;
	vk::Extent2D                    m_Extent;
	vk::Format                      m_Format;
	vk::ImageUsageFlags             m_ImageUsage;
	vk::SurfaceTransformFlagBitsKHR m_PreTransform;
	vk::PresentModeKHR              m_PresentMode;
	vk::ArrayProxy<const vk::Image> m_Images;
	uint32_t                        m_ImageCount;
};
} // namespace vulkan