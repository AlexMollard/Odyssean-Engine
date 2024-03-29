#include "pch.h"

#include "DescriptorManager.h"

#include <iostream>

#include "Engine/VulkanAPI/Types/VkContainer.h"


VulkanWrapper::DescriptorManager::DescriptorManager(vk::Device device) : m_device(device), m_descriptorLayoutCache(device)
{
	// create descriptor pool
	std::vector<vk::DescriptorPoolSize> poolSizes = { { vk::DescriptorType::eCombinedImageSampler, 128 }, { vk::DescriptorType::eUniformBuffer, 128 } };

	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();
	poolInfo.maxSets       = 32;
	poolInfo.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

	m_descriptorPool = m_device.createDescriptorPool(poolInfo);

	createBaseDescriptorLayouts();

	setMinUniformBufferOffsetAlignment();
}

std::shared_ptr<VulkanWrapper::DescriptorSetLayout> VulkanWrapper::DescriptorManager::createDescriptorSetLayout(std::vector<VulkanWrapper::BindingData> bindings)
{
	return m_descriptorLayoutCache.getDescriptorSetLayout(bindings);
}

std::vector<vk::DescriptorSet> VulkanWrapper::DescriptorManager::allocateDescriptorSets(VulkanWrapper::DescriptorSetLayout descriptorSetLayout, uint32_t descriptorSetCount)
{
	std::vector<vk::DescriptorSetLayout> layouts(descriptorSetCount, descriptorSetLayout.GetLayout());
	vk::DescriptorSetAllocateInfo allocInfo(m_descriptorPool, layouts.size(), layouts.data());

	std::vector<vk::DescriptorSet> descriptorSets = m_device.allocateDescriptorSets(allocInfo);

	for (auto& descriptorSet : descriptorSets)
	{
		m_descriptorSets.push_back(descriptorSet);
	}

	return descriptorSets;
}

std::vector<vk::DescriptorSet> VulkanWrapper::DescriptorManager::allocateDescriptorSets(std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> descriptorSetLayouts,
                                                                                        uint32_t descriptorSetCount)
{
	std::vector<vk::DescriptorSetLayout> layouts;
	layouts.reserve(descriptorSetLayouts.size());

	for (auto& descriptorSetLayout : descriptorSetLayouts)
	{
		layouts.push_back(descriptorSetLayout->GetLayout());
	}

	vk::DescriptorSetAllocateInfo allocInfo(m_descriptorPool, layouts.size(), layouts.data());

	std::vector<vk::DescriptorSet> descriptorSets = m_device.allocateDescriptorSets(allocInfo);

	for (auto& descriptorSet : descriptorSets)
	{
		m_descriptorSets.push_back(descriptorSet);
	}

	return descriptorSets;
}

void VulkanWrapper::DescriptorManager::updateDescriptorSets(std::vector<vk::WriteDescriptorSet> descriptorWrites)
{
	m_device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void VulkanWrapper::DescriptorManager::cleanup()
{
	for (auto& descriptorSet : m_descriptorSets)
	{
		m_device.freeDescriptorSets(m_descriptorPool, descriptorSet);
	}
	m_device.destroyDescriptorPool(m_descriptorPool);

	m_descriptorSets.clear();
}

std::shared_ptr<VulkanWrapper::DescriptorSetLayout> VulkanWrapper::DescriptorManager::getLayout(const char* name)
{
	return m_descriptorLayoutCache.getDescriptorSetLayout(name);
}

void VulkanWrapper::DescriptorManager::createBaseDescriptorLayouts()
{
	// MVP
	std::vector<BindingData> mvpBindings = { { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex } };

	// Create the layout and add it to both the hash map and string map
	m_descriptorLayoutCache.addDescriptorSetLayout("MVPLayout", createDescriptorSetLayout(mvpBindings));

	// ALL LIGHTS
	std::vector<BindingData> allLightsBindings = {
		{ 0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment }, // spotLights
		//{ 1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment }, // pointLights
		//{ 2, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment }, // directionalLights
	};

	// Create the layout and add it to both the hash map and string map
	m_descriptorLayoutCache.addDescriptorSetLayout("AllLightsLayout", createDescriptorSetLayout(allLightsBindings));

	// MATERIAL
	std::vector<BindingData> materialBindings = {
		{ 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // diffuseMap
		{ 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // specularMap
		{ 2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // normalMap
		{ 3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // heightMap
		{ 4, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // ambientMap
		{ 5, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // emissiveMap
		{ 6, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // shininessMap
		{ 7, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // opacityMap
		{ 8, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // displacementMap
		{ 9, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // lightMap
		{ 10, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment } // reflectionMap
	};

	// Create the layout and add it to both the hash map and string map
	m_descriptorLayoutCache.addDescriptorSetLayout("MaterialLayout", createDescriptorSetLayout(materialBindings));

	// BASIC PBR MATERIAL
	std::vector<BindingData> pbrMaterialBindings = {
		{ 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // diffuseMap
		{ 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // normalMap
		{ 2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // roughnessMap
		{ 3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // metallicMap
		{ 4, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment }, // aoMap
	};

	// Create the layout and add it to both the hash map and string map
	m_descriptorLayoutCache.addDescriptorSetLayout("PBRMaterialLayout", createDescriptorSetLayout(pbrMaterialBindings));

	// SKYBOX
	std::vector<BindingData> skyboxBindings = { { 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment } };

	// Create the layout and add it to both the hash map and string map
	m_descriptorLayoutCache.addDescriptorSetLayout("SkyboxLayout", createDescriptorSetLayout(skyboxBindings));
}

void VulkanWrapper::DescriptorManager::freeDescriptorSets(std::vector<vk::DescriptorSet> descriptorSets)
{
	m_device.freeDescriptorSets(m_descriptorPool, descriptorSets);
}

// STATIC FUNCTIONS
std::vector<vk::DescriptorSetLayout> VulkanWrapper::DescriptorManager::GetLayoutData(
    const std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>>& descriptorSetLayouts)
{
	std::vector<vk::DescriptorSetLayout> layouts;
	layouts.reserve(descriptorSetLayouts.size());

	for (const auto& descriptorSetLayout : descriptorSetLayouts)
	{
		layouts.push_back(descriptorSetLayout->GetLayout());
	}

	return layouts;
}

void VulkanWrapper::DescriptorManager::DebugLayouts(std::vector<std::shared_ptr<VulkanWrapper::DescriptorSetLayout>> descriptorSetLayouts)
{
	for (auto& descriptorSetLayout : descriptorSetLayouts)
	{
		descriptorSetLayout->Print();
		std::cout << std::endl;
	}
}

size_t VulkanWrapper::DescriptorManager::GetMinUniformBufferSize(size_t size)
{
	// Find the smallest size that is a multiple of the minimum required alignment
	size_t minUboAlignment = m_minUniformBufferOffsetAlignment;
	size_t alignedSize     = size;
	if (minUboAlignment > 0)
	{
		alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	return alignedSize;
}

size_t VulkanWrapper::DescriptorManager::setMinUniformBufferOffsetAlignment()
{
	m_minUniformBufferOffsetAlignment = VkContainer::instance().deviceQueue.m_PhysicalDevice.getProperties().limits.minUniformBufferOffsetAlignment;

	return m_minUniformBufferOffsetAlignment;
}
size_t VulkanWrapper::DescriptorManager::GetMinUniformBufferOffsetAlignment()
{
	return m_minUniformBufferOffsetAlignment;
}