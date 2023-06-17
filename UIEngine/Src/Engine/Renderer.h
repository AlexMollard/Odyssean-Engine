#pragma once
#include "Engine/VulkanAPI/Helpers/VkRenderHelper.h"
#include "glm/fwd.hpp"
#include <Engine/ErrorHandling.h>
#include <memory>

namespace VulkanWrapper
{
struct VkContainer;
struct Mesh;
} // namespace VulkanWrapper

namespace node
{
class Camera;
}

class Renderer
{
public:
	enum class API
	{
		OPENGL,
		VULKAN,
		DIRECTX
	};

	struct Impl
	{
		virtual void Draw() const = 0;
		virtual void CleanUp() const = 0;
	};

	struct OpenGLImpl : public Impl
	{
		void Draw() const override;
		void CleanUp() const override;
	};

	struct VulkanImpl : public Impl
	{
		// Creates the m_RenderHelper
		virtual ~VulkanImpl() = default;

		void InitRenderer();

		void Draw() const override;

		void AddMesh(const VulkanWrapper::Mesh& mesh, const glm::mat4& model) const;

		void AddDirLight(const DirectionalLight& light) const;
		void AddPointLight(const PointLight& light) const;
		void AddSpotLight(const SpotLight& light) const;

		void SetCamera(node::Camera& camera);

		void CleanUp() const override;
	private:
		VkRenderHelper* m_RenderHelper = nullptr;
	};

	struct DirectXImpl : public Impl
	{
		void Draw() const override;
		void CleanUp() const override;
	};

	// Renderer class starts here

	explicit Renderer(API api) : current_api(api)
	{
		switch (current_api)
		{
		case API::OPENGL: impl = std::make_unique<OpenGLImpl>(); break;
		case API::VULKAN: impl = std::make_unique<VulkanImpl>(); break;
		case API::DIRECTX: impl = std::make_unique<DirectXImpl>(); break;
		}
	}

	void Draw() const
	{
		impl->Draw();
	}

	std::unique_ptr<Impl>& GetImpl()
	{
		return impl;
	};

private:
	API current_api;
	std::unique_ptr<Impl> impl;
};
