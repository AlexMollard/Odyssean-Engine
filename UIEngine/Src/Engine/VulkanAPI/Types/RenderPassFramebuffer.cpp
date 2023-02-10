#include "pch.h"

#include "RenderPassFramebuffer.h"
namespace VulkanWrapper
{
RenderPassFramebuffer::RenderPassFramebuffer(vk::RenderPass renderPass, std::vector<vk::Framebuffer> framebuffers)
{
	m_RenderPass   = renderPass;
	m_Framebuffers = framebuffers;
}

bool RenderPassFramebuffer::BeginRenderPass(vk::CommandBuffer commandBuffer, vk::SubpassContents subpassContents, uint32_t framebufferIndex, vk::Rect2D renderArea,
                                            std::vector<vk::ClearValue> clearValues)
{
	vk::RenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.renderPass      = m_RenderPass;
	renderPassBeginInfo.framebuffer     = m_Framebuffers[framebufferIndex];
	renderPassBeginInfo.renderArea      = renderArea;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues    = clearValues.data();

	commandBuffer.beginRenderPass(&renderPassBeginInfo, subpassContents);

	return true;
}

bool RenderPassFramebuffer::EndRenderPass(vk::CommandBuffer commandBuffer)
{
	commandBuffer.endRenderPass();

	return true;
}
} // namespace VulkanWrapper