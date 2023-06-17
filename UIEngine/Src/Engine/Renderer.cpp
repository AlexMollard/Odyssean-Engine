#include "Renderer.h"

#include "VulkanAPI/Helpers/VkRenderHelper.h"

#include "Engine/VulkanAPI/Types/VkContainer.h"
#include "Engine/VulkanAPI/Types/common.h"

void Renderer::OpenGLImpl::Draw() const
{
	// Load the current model, view, and projection matrices from the rendering state
	// Bind the vertex and index buffers for the current model
	// Configure the shaders for the current lighting and material parameters
	// Issue the draw call to render the model
	// Unbind the buffers and shaders
}

void Renderer::OpenGLImpl::CleanUp() const {}

void Renderer::VulkanImpl::InitRenderer()
{
	m_RenderHelper = new VkRenderHelper;
}

void Renderer::VulkanImpl::Draw() const
{
	m_RenderHelper->DrawFrame();
}

void Renderer::VulkanImpl::AddMesh(const VulkanWrapper::Mesh& mesh, const glm::mat4& model) const
{
	m_RenderHelper->AddMesh(mesh, model);
}

void Renderer::VulkanImpl::AddDirLight(const DirectionalLight& light) const
{
	// Cast light to void* and pass it to the render helper
	auto* lightPtr = (void*)&light;
	m_RenderHelper->AddLight(LIGHT_TYPE::DIRECTIONAL, lightPtr);
}

void Renderer::VulkanImpl::AddPointLight(const PointLight& light) const
{
	// Cast light to void* and pass it to the render helper
	auto* lightPtr = (void*)&light;
	m_RenderHelper->AddLight(LIGHT_TYPE::POINT, lightPtr);
}

void Renderer::VulkanImpl::AddSpotLight(const SpotLight& light) const
{
	// Cast light to void* and pass it to the render helper
	auto* lightPtr = (void*)&light;
	m_RenderHelper->AddLight(LIGHT_TYPE::SPOT, lightPtr);
}

void Renderer::VulkanImpl::SetCamera(node::Camera& camera)
{
	m_RenderHelper->SetCamera(&camera);
}

void Renderer::VulkanImpl::CleanUp() const
{
	m_RenderHelper->ClearMeshes();
	m_RenderHelper->ClearLights();
	delete m_RenderHelper;
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

void Renderer::DirectXImpl::CleanUp() const {}
