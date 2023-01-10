#include "CommandBuffer.h"
namespace vulkan
{

void CommandBuffer::Create(vk::Device device, vk::CommandPool commandPool, vk::CommandBufferLevel level /*= ePrimary*/)
{
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = 1;

    m_CommandBuffer = device.allocateCommandBuffers(allocInfo)[0];
}

void CommandBuffer::Destroy()
{
    m_CommandBuffer.reset();
}

void CommandBuffer::Begin(vk::CommandBufferUsageFlags flags /*= eOneTimeSubmit*/) const
{
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = flags;
    m_CommandBuffer.begin(beginInfo);
}

void CommandBuffer::End() const
{
    m_CommandBuffer.end();
}

void CommandBuffer::Reset(vk::CommandBufferResetFlags flags /*= eReleaseResources*/) const
{
    m_CommandBuffer.reset(flags);
}

void CommandBuffer::BeginRenderPass(vk::RenderPass renderPass, vk::Framebuffer framebuffer, vk::Rect2D renderArea, vk::ArrayProxy<const vk::ClearValue> clearValues,
									vk::SubpassContents subpassContents /*= eInline*/) const
{
    vk::RenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea = renderArea;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    m_CommandBuffer.beginRenderPass(&renderPassInfo, subpassContents);
}

void CommandBuffer::NextSubpass(vk::SubpassContents subpassContents /*= eInline*/) const
{
    m_CommandBuffer.nextSubpass(subpassContents);
}

void CommandBuffer::EndRenderPass() const
{
    m_CommandBuffer.endRenderPass();
}

void CommandBuffer::Record(vk::CommandBufferUsageFlags flags, std::function<void()> lambda) const
{
    Begin(flags);
    lambda();
    End();
}

void CommandBuffer::BindPipeline(vk::PipelineBindPoint pipelineBindPoint, vk::Pipeline pipeline) const
{
    m_CommandBuffer.bindPipeline(pipelineBindPoint, pipeline);
}

void CommandBuffer::BindVertexBuffers(uint32_t firstBinding, vk::ArrayProxy<const vk::Buffer> buffers, vk::ArrayProxy<const vk::DeviceSize> offsets) const
{
    m_CommandBuffer.bindVertexBuffers(firstBinding, buffers, offsets);
}

void CommandBuffer::BindIndexBuffer(vk::Buffer buffer, vk::DeviceSize offset, vk::IndexType indexType) const
{
    m_CommandBuffer.bindIndexBuffer(buffer, offset, indexType);
}

void CommandBuffer::BindDescriptorSets(vk::PipelineBindPoint pipelineBindPoint, vk::PipelineLayout layout, uint32_t firstSet, vk::ArrayProxy<const vk::DescriptorSet> descriptorSets, vk::ArrayProxy<const uint32_t> dynamicOffsets) const
{
    m_CommandBuffer.bindDescriptorSets(pipelineBindPoint, layout, firstSet, descriptorSets, dynamicOffsets);
}

void CommandBuffer::SetViewport(uint32_t firstViewport, vk::ArrayProxy<const vk::Viewport> viewports) const
{
    m_CommandBuffer.setViewport(firstViewport, viewports);
}

void CommandBuffer::SetScissor(uint32_t firstScissor, vk::ArrayProxy<const vk::Rect2D> scissors) const
{
    m_CommandBuffer.setScissor(firstScissor, scissors);
}

void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const
{
    m_CommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const
{
    m_CommandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::DrawIndirect(vk::Buffer buffer, vk::DeviceSize offset, uint32_t drawCount, uint32_t stride) const
{
    m_CommandBuffer.drawIndirect(buffer, offset, drawCount, stride);
}

void CommandBuffer::DrawIndexedIndirect(vk::Buffer buffer, vk::DeviceSize offset, uint32_t drawCount, uint32_t stride) const
{
    m_CommandBuffer.drawIndexedIndirect(buffer, offset, drawCount, stride);
}

vk::CommandBuffer CommandBuffer::getCommandBuffer() const
{
    return m_CommandBuffer;
}

} // namespace vulkan