#pragma once
#include "vulkan/vulkan.hpp"
namespace vulkan
{
class RenderPassFramebuffer
{
public:
	RenderPassFramebuffer() = default;
	RenderPassFramebuffer(vk::RenderPass renderPass, vk::Framebuffer framebuffer);

	// Getters
	vk::RenderPass  getRenderPass() const;
	vk::Framebuffer getFramebuffer() const;

	// Setters
	void setRenderPass(vk::RenderPass renderPass);
	void setFramebuffer(vk::Framebuffer framebuffer);

	// Helper functions
	void beginRenderPass(vk::CommandBuffer commandBuffer, vk::Rect2D renderArea, vk::ArrayProxy<const vk::ClearValue> clearValues, vk::SubpassContents subpassContents = vk::SubpassContents::eInline) const;
	void nextSubpass(vk::CommandBuffer commandBuffer, vk::SubpassContents subpassContents = vk::SubpassContents::eInline) const;
	void endRenderPass(vk::CommandBuffer commandBuffer) const;

private:
	vk::RenderPass  m_RenderPass;
	vk::Framebuffer m_Framebuffer;
};
} // namespace vulkan