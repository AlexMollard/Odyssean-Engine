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

// The lights must align to a total of 32 bytes (color + position + intensity = 12 + 12 + 4 = 28 bytes) so we add 4 bytes of padding
struct PointLight
{
	glm::vec3 color    = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 position = glm::vec3(0.0f);
	float intensity    = 1.0f;

	PointLight() = default;
	PointLight(const glm::vec3& color, const glm::vec3& position, float intensity) noexcept : color(color), position(position), intensity(intensity) {}
};

struct DirectionalLight
{
	alignas(16) glm::vec3 color;
	alignas(16) glm::vec3 direction;
	alignas(4) float intensity;

	DirectionalLight() : color(glm::vec3(4444.0f)), direction(glm::vec3(4444.0f)), intensity(0.0f) {}
};

struct SpotLight
{
	alignas(16) glm::vec3 color;
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 direction;
	alignas(4) float intensity;
	alignas(4) float cutoff;

	SpotLight() : color(glm::vec3(4444.0f)), position(glm::vec3(4444.0f)), direction(glm::vec3(4444.0f)), intensity(0.0f), cutoff(4444.0f) {}
};

namespace VulkanWrapper
{
struct VkContainer;
}

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

namespace VulkanWrapper
{
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