#pragma once
#include <vulkan/vulkan.hpp>
namespace VulkanWrapper
{
struct RenderPassFramebuffer
{
	RenderPassFramebuffer() = default;
	RenderPassFramebuffer(vk::RenderPass renderPass, std::vector<vk::Framebuffer> framebuffer);

	// Helper functions
	bool BeginRenderPass(vk::CommandBuffer commandBuffer, vk::SubpassContents subpassContents, uint32_t framebufferIndex, vk::Rect2D renderArea, std::vector<vk::ClearValue> clearValues);
	bool EndRenderPass(vk::CommandBuffer commandBuffer);

	vk::RenderPass               m_RenderPass;
	std::vector<vk::Framebuffer> m_Framebuffers;
};
} // namespace VulkanWrapper