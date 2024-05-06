#pragma once
#include "glm/fwd.hpp"
#include <Engine/ErrorHandling.h>
#include <memory>

#include "Engine/VulkanAPI/Helpers/VkRenderHelper.h"

namespace VulkanWrapper
{
class VkContainer;
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

	struct ImplBase
	{
		virtual void Draw() const    = 0;
		virtual void CleanUp() const = 0;
		virtual ~ImplBase()          = default;
	};

	struct OpenGLImpl : public ImplBase
	{
		~OpenGLImpl() final = default;

		void Draw() const override;
		void CleanUp() const override;
	};

	struct VulkanImpl : public ImplBase
	{
		~VulkanImpl() final = default;

		void InitRenderer();

		void Draw() const override;

		void AddMesh(const VulkanWrapper::Mesh& mesh, const glm::mat4& model) const;

		void AddDirLight(const DirectionalLight* light) const;
		void AddPointLight(const PointLight* light) const;
		void AddSpotLight(const SpotLight* light) const;

		void SetCamera(node::Camera& camera);

		void CleanUp() const override;

	private:
		std::unique_ptr<VkRenderHelper> m_RenderHelper;
	};

	struct DirectXImpl : public ImplBase
	{
		~DirectXImpl() final = default;

		void Draw() const override;
		void CleanUp() const override;
	};

	explicit Renderer(API api)
	{
		switch (api)
		{
		case API::OPENGL:
		{
			m_impl = std::make_unique<OpenGLImpl>();
			break;
		}
		case API::VULKAN:
		{
			m_impl = std::make_unique<VulkanImpl>();
			break;
		}
		case API::DIRECTX:
		{
			m_impl = std::make_unique<DirectXImpl>();
			break;
		}

		default:
		{
			S_ASSERT(false, "You didnt pass a valid API");
		}
		};
	}

	void Draw() const
	{
		m_impl->Draw();
	}

	std::unique_ptr<ImplBase>& GetImpl()
	{
		return m_impl;
	};

	std::unique_ptr<ImplBase> m_impl;
};
