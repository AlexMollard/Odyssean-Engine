#include "RenderPassFramebuffer.h"
namespace vulkan
{
RenderPassFramebuffer::RenderPassFramebuffer(vk::RenderPass renderPass, vk::Framebuffer framebuffer)
{
	m_RenderPass  = renderPass;
	m_Framebuffer = framebuffer;
}

vk::RenderPass RenderPassFramebuffer::getRenderPass() const
{
	return m_RenderPass;
}

vk::Framebuffer RenderPassFramebuffer::getFramebuffer() const
{
	return m_Framebuffer;
}

void RenderPassFramebuffer::setRenderPass(vk::RenderPass renderPass)
{
	m_RenderPass = renderPass;
}

void RenderPassFramebuffer::setFramebuffer(vk::Framebuffer framebuffer)
{
	m_Framebuffer = framebuffer;
}

void RenderPassFramebuffer::beginRenderPass(vk::CommandBuffer commandBuffer, vk::Rect2D renderArea, vk::ArrayProxy<const vk::ClearValue> clearValues, vk::SubpassContents subpassContents /*= vk::SubpassContents::eInline*/) const
{
	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass              = m_RenderPass;
	renderPassInfo.framebuffer             = m_Framebuffer;
	renderPassInfo.renderArea              = renderArea;
	renderPassInfo.clearValueCount         = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues            = clearValues.data();

	commandBuffer.beginRenderPass(&renderPassInfo, subpassContents);
}

void RenderPassFramebuffer::nextSubpass(vk::CommandBuffer commandBuffer, vk::SubpassContents subpassContents /*= vk::SubpassContents::eInline*/) const
{
	commandBuffer.nextSubpass(subpassContents);
}

void RenderPassFramebuffer::endRenderPass(vk::CommandBuffer commandBuffer) const
{
	commandBuffer.endRenderPass();
}
} // namespace vulkan