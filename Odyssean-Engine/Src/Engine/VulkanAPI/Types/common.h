#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

enum class LIGHT_TYPE
{
	DIRECTIONAL,
	POINT,
	SPOT
};

constexpr int MAX_POINT_LIGHTS       = 4;
constexpr int MAX_DIRECTIONAL_LIGHTS = 4;
constexpr int MAX_SPOT_LIGHTS        = 4;

struct BaseLight
{
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);
};

struct PointLight : public BaseLight
{
	glm::vec3 position = glm::vec3(0.0f);
	float intensity    = 1.0f;

	PointLight() = default;
	PointLight(glm::vec3 position, glm::vec3 color, float intensity) : position(position), intensity(intensity)
	{
		this->color = color;
	}
};

struct DirectionalLight : public BaseLight
{
	glm::vec3 direction;
	float intensity;

	DirectionalLight() = default;
	DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity) : direction(direction), intensity(intensity)
	{
		this->color = color;
	}
};

struct SpotLight : public BaseLight
{
	glm::vec3 position;
	glm::vec3 direction;
	float intensity;
	float cutoff;

	SpotLight() = default;
	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity, float cutoff)
		: position(position), direction(direction), intensity(intensity), cutoff(cutoff)
	{
		this->color = color;
	}
};

namespace VulkanWrapper
{
class VkContainer;
class DeviceQueue;
struct Buffer
{
	vk::Buffer buffer;
	vk::DeviceMemory memory;

	vk::DeviceSize bufferSize;
	vk::BufferUsageFlags usage;
	vk::MemoryPropertyFlags properties;

	vk::DeviceSize alignment;

	vk::MappedMemoryRange Update(vk::Device& device, const void* data, vk::DeviceSize size);
};

struct Texture
{
	vk::Image image;
	vk::DeviceMemory memory;
	vk::ImageView imageView;
	vk::Sampler sampler;
	vk::DescriptorImageInfo descriptorImageInfo;

	void Create(VulkanWrapper::VkContainer& api, vk::Queue queue, const void* data, uint32_t width, uint32_t height);
	static VulkanWrapper::Texture* Load(VulkanWrapper::VkContainer& api, vk::Queue queue, const char* dir);

	vk::Result destroy(vk::Device& device);
	const vk::DescriptorImageInfo& GetDescriptorImageInfo() const;

	static void CreateDebugMetalnessTexture(const char* dir);
	static void CreateDebugRoughnessTexture(const char* dir);
	static void CreateDebugAmbientTexture(const char* dir);
};
} // namespace VulkanWrapper

// Error checking
#define VK_CHECK_RESULT(f)                                                                                                                                                    \
	{                                                                                                                                                                         \
		vk::Result res = (f);                                                                                                                                                 \
		if (res != vk::Result::eSuccess)                                                                                                                                      \
		{                                                                                                                                                                     \
			printf("Fatal : VkResult is %d in %s at line %d\n", res, __FILE__, __LINE__);                                                                                     \
			assert(res == vk::Result::eSuccess);                                                                                                                              \
		}                                                                                                                                                                     \
	}