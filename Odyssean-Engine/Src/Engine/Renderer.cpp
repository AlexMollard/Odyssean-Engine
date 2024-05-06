#include "Renderer.h"

#include "VulkanAPI/Helpers/VkRenderHelper.h"

#include "Engine/VulkanAPI/Types/common.h"

void Renderer::OpenGLImpl::Draw() const
{
	// Load the current model, view, and projection matrices from the rendering state
	// Bind the vertex and index buffers for the current model
	// Configure the shaders for the current lighting and material parameters
	// Issue the draw call to render the model
	// Unbind the buffers and shaders
}

void Renderer::OpenGLImpl::CleanUp() const 
{
	// OpenGl not yet set up
}

void Renderer::VulkanImpl::InitRenderer()
{
	m_RenderHelper = std::make_unique<VkRenderHelper>();
}

void Renderer::VulkanImpl::Draw() const
{
	m_RenderHelper->DrawFrame();
}

void Renderer::VulkanImpl::AddMesh(const VulkanWrapper::Mesh& mesh, const glm::mat4& model) const
{
	m_RenderHelper->AddMesh(mesh, model);
}

void Renderer::VulkanImpl::AddDirLight(const DirectionalLight* light) const
{
	// Cast light to void* and pass it to the render helper
	m_RenderHelper->AddLight(LIGHT_TYPE::DIRECTIONAL, static_cast<const BaseLight*>(light));
}

void Renderer::VulkanImpl::AddPointLight(const PointLight* light) const
{
	// Cast light to void* and pass it to the render helper
	m_RenderHelper->AddLight(LIGHT_TYPE::POINT, static_cast<const BaseLight*>(light));
}

void Renderer::VulkanImpl::AddSpotLight(const SpotLight* light) const
{
	// Cast light to void* and pass it to the render helper
	m_RenderHelper->AddLight(LIGHT_TYPE::SPOT, static_cast<const BaseLight*>(light));
}

void Renderer::VulkanImpl::SetCamera(node::Camera& camera)
{
	m_RenderHelper->SetCamera(&camera);
}

void Renderer::VulkanImpl::CleanUp() const
{
	m_RenderHelper->ClearMeshes();
	m_RenderHelper->ClearLights();
}

void Renderer::DirectXImpl::Draw() const
{
	// Load the current model, view, and projection matrices into the shader constants
	// Bind the vertex and index buffers for the current model
	// Configure the shaders for the current lighting and material parameters
	// Issue the draw call to render the model
	// Unbind the buffers and shaders
	S_ASSERT(false, "DirectX is not yet supported")
}

void Renderer::DirectXImpl::CleanUp() const 
{
	// DirectX not yet set up
}
