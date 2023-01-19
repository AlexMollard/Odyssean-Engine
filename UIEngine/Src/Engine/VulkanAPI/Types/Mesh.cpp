#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace vulkan
{
std::vector<vk::VertexInputAttributeDescription> Vertex::GetVertexAttributes()
{
	std::vector<vk::VertexInputAttributeDescription> vertexAttributes(4);
	vertexAttributes[0].binding  = 0;
	vertexAttributes[0].location = 0;
	vertexAttributes[0].format   = vk::Format::eR32G32B32Sfloat;
	vertexAttributes[0].offset   = offsetof(Vertex, pos);

	vertexAttributes[1].binding  = 0;
	vertexAttributes[1].location = 1;
	vertexAttributes[1].format   = vk::Format::eR32G32B32Sfloat;
	vertexAttributes[1].offset   = offsetof(Vertex, normal);

	vertexAttributes[2].binding  = 0;
	vertexAttributes[2].location = 2;
	vertexAttributes[2].format   = vk::Format::eR32G32Sfloat;
	vertexAttributes[2].offset   = offsetof(Vertex, texCoord);

	vertexAttributes[3].binding  = 0;
	vertexAttributes[3].location = 3;
	vertexAttributes[3].format   = vk::Format::eR32G32B32Sfloat;
	vertexAttributes[3].offset   = offsetof(Vertex, color);

	return vertexAttributes;
}

vk::VertexInputBindingDescription Vertex::GetBindingDescription()
{
	vk::VertexInputBindingDescription bindingDescription;
	bindingDescription.binding   = 0;
	bindingDescription.stride    = sizeof(Vertex);
	bindingDescription.inputRate = vk::VertexInputRate::eVertex;
	return bindingDescription;
}

void Mesh::Destroy(vk::Device& device)
{
	// Vertex buffer
	device.freeMemory(vertexBuffer.memory);
	device.destroyBuffer(vertexBuffer.buffer);

	// Index buffer
	device.freeMemory(indexBuffer.memory);
	device.destroyBuffer(indexBuffer.buffer);

	// mvp matrix buffer
	device.freeMemory(mvpMatrixBuffer.memory);
	device.destroyBuffer(mvpMatrixBuffer.buffer);

	// light properties buffer
	device.freeMemory(lightPropertiesBuffer.memory);
	device.destroyBuffer(lightPropertiesBuffer.buffer);

	// descriptor pool
	device.destroyDescriptorPool(descriptorPool);

	// descriptor set layout
	device.destroyDescriptorSetLayout(descriptorSetLayout);

	// Texture
	texture.destroy(device);
}

vk::Result Mesh::LoadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { return vk::Result::eErrorInitializationFailed; }
	directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene);
	return vk::Result::eSuccess;
}

void Mesh::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) { ProcessNode(node->mChildren[i], scene); }
}

void Mesh::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.pos      = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertex.normal   = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		vertex.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) { indices.push_back(face.mIndices[j]); }
	}
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString    texPath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) { texturePath = directory + "/" + texPath.C_Str(); }
	}
}

void Mesh::Create(API& api)
{
	CreateVertexBuffer(api.deviceQueue, api.commandPool);
	CreateIndexBuffer(api.deviceQueue, api.commandPool);
	CreateMVPMatrixBuffer(api.deviceQueue, api.commandPool);
	CreateLightPropertiesBuffer(api.deviceQueue, api.commandPool, lightProperties);
	CreateDescriptorSetLayout(api.deviceQueue.m_Device);
	CreateDescriptorPool(api.deviceQueue.m_Device);
	CreateDescriptorSet(api.deviceQueue, api, api.graphicsPipelineLayout);
}

void Mesh::AddToCommandBuffer(vk::Device& device, vk::CommandBuffer commandBuffer, vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout)
{
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, &vertexBuffer.buffer, offsets);
	commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	commandBuffer.drawIndexed(indices.size(), 1, 0, 0, 0);
}

void Mesh::CreateVertexBuffer(DeviceQueue& devices, vk::CommandPool& commandPool)
{
	vk::Result res =
		devices.CreateBuffer(vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, vertexBuffer, vertices.data(), sizeof(Vertex) * vertices.size());
	if (res != vk::Result::eSuccess) { throw std::runtime_error("failed to create vertex buffer!"); }
}

void Mesh::CreateIndexBuffer(DeviceQueue& devices, vk::CommandPool& commandPool)
{
	vk::Result res =
		devices.CreateBuffer(vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, indexBuffer, indices.data(), sizeof(uint32_t) * indices.size());
	if (res != vk::Result::eSuccess) { throw std::runtime_error("failed to create index buffer!"); }
}

void Mesh::CreateMVPMatrixBuffer(DeviceQueue& devices, vk::CommandPool& commandPool)
{
	vk::Result res = devices.CreateBuffer(vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mvpMatrixBuffer, &mvpMatrix, sizeof(mvpMatrix));
	if (res != vk::Result::eSuccess) { throw std::runtime_error("failed to create model matrix buffer!"); }
}

void Mesh::CreateLightPropertiesBuffer(DeviceQueue& devices, vk::CommandPool& commandPool, const LightProperties& lightProperties)
{
	// Create buffer
	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.size        = sizeof(lightProperties);
	bufferCreateInfo.usage       = vk::BufferUsageFlagBits::eUniformBuffer;
	lightPropertiesBuffer.buffer = devices.m_Device.createBuffer(bufferCreateInfo);

	// Allocate memory for buffer
	vk::MemoryRequirements memoryRequirements = devices.m_Device.getBufferMemoryRequirements(lightPropertiesBuffer.buffer);
	vk::MemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;

	uint32_t                memoryTypeBits = memoryRequirements.memoryTypeBits;
	vk::MemoryPropertyFlags properties     = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	devices.FindMemoryType(memoryTypeBits, properties, memoryAllocateInfo.memoryTypeIndex);
	lightPropertiesBuffer.memory = devices.m_Device.allocateMemory(memoryAllocateInfo);

	// Bind buffer to allocated memory
	devices.m_Device.bindBufferMemory(lightPropertiesBuffer.buffer, lightPropertiesBuffer.memory, 0);

	// Copy light properties data to buffer
	void* data;
	devices.m_Device.mapMemory(lightPropertiesBuffer.memory, 0, sizeof(lightProperties), {}, &data);
	memcpy(data, &lightProperties, sizeof(lightProperties));
	devices.m_Device.unmapMemory(lightPropertiesBuffer.memory);

	// Get the minimum uniform buffer offset alignment value
	vk::PhysicalDeviceProperties physicalDeviceProperties        = devices.m_PhysicalDevice.getProperties();
	uint32_t                     minUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;

	// Align the offset value to a multiple of the minimum uniform buffer offset alignment
	lightPropertiesBufferInfo.offset = 0;
	lightPropertiesBufferInfo.offset = (lightPropertiesBufferInfo.offset + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1);
	lightPropertiesBufferInfo.buffer = lightPropertiesBuffer.buffer;
	lightPropertiesBufferInfo.range  = sizeof(lightProperties);
}

void Mesh::CreateDescriptorSetLayout(vk::Device& device)
{
	// uniform buffer descriptor layout
	vk::DescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding                        = 0;
	uboLayoutBinding.descriptorType                 = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.descriptorCount                = 1;
	uboLayoutBinding.stageFlags                     = vk::ShaderStageFlagBits::eVertex;
	uboLayoutBinding.pImmutableSamplers             = nullptr;

	// sampler descriptor layout
	vk::DescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding                        = 1;
	samplerLayoutBinding.descriptorCount                = 1;
	samplerLayoutBinding.descriptorType                 = vk::DescriptorType::eCombinedImageSampler;
	samplerLayoutBinding.pImmutableSamplers             = nullptr;
	samplerLayoutBinding.stageFlags                     = vk::ShaderStageFlagBits::eFragment;

	// light properties descriptor layout
	vk::DescriptorSetLayoutBinding lightPropertiesLayoutBinding = {};
	lightPropertiesLayoutBinding.binding                        = 2;
	lightPropertiesLayoutBinding.descriptorCount                = 1;
	lightPropertiesLayoutBinding.descriptorType                 = vk::DescriptorType::eUniformBuffer;
	lightPropertiesLayoutBinding.pImmutableSamplers             = nullptr;
	lightPropertiesLayoutBinding.stageFlags                     = vk::ShaderStageFlagBits::eFragment;

	std::array<vk::DescriptorSetLayoutBinding, 3> bindings = { uboLayoutBinding, samplerLayoutBinding, lightPropertiesLayoutBinding };

	vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.bindingCount                      = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings                         = bindings.data();

	descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
}

void Mesh::CreateDescriptorPool(vk::Device& device)
{
	vk::DescriptorPoolSize uboPoolSize = {};
	uboPoolSize.type                   = vk::DescriptorType::eUniformBuffer;
	uboPoolSize.descriptorCount        = 1;

	vk::DescriptorPoolSize lightPropertiesPoolSize = {};
	lightPropertiesPoolSize.type                   = vk::DescriptorType::eUniformBuffer;
	lightPropertiesPoolSize.descriptorCount        = 1;

	vk::DescriptorPoolSize samplerPoolSize = {};
	samplerPoolSize.type                   = vk::DescriptorType::eCombinedImageSampler;
	samplerPoolSize.descriptorCount        = 1;

	std::array<vk::DescriptorPoolSize, 3> poolSizes = { uboPoolSize, lightPropertiesPoolSize, samplerPoolSize };
	vk::DescriptorPoolCreateInfo          poolInfo  = {};
	poolInfo.poolSizeCount                          = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes                             = poolSizes.data();
	poolInfo.maxSets                                = 3;

	descriptorPool = device.createDescriptorPool(poolInfo);
}

void Mesh::CreateDescriptorSet(DeviceQueue& devices, vulkan::API& api, vk::PipelineLayout& graphicsPipelineLayout)
{
	vk::DescriptorSetAllocateInfo allocInfo = {};
	allocInfo.descriptorPool                = descriptorPool;
	allocInfo.descriptorSetCount            = 1;
	allocInfo.pSetLayouts                   = &descriptorSetLayout;

	descriptorSet = devices.m_Device.allocateDescriptorSets(allocInfo)[0];

	vk::DescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer                   = mvpMatrixBuffer.buffer;
	bufferInfo.offset                   = 0;
	bufferInfo.range                    = sizeof(ModelViewProjection);

	std::array<vk::WriteDescriptorSet, 3> descriptorWrites = {};

	descriptorWrites[0].dstSet          = descriptorSet;
	descriptorWrites[0].dstBinding      = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo     = &bufferInfo;

	// Light properties
	vk::PhysicalDeviceProperties physicalDeviceProperties        = devices.m_PhysicalDevice.getProperties();
	uint32_t                     minUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;

	vk::DescriptorBufferInfo lightPropertiesBufferInfo = {};
	lightPropertiesBufferInfo.buffer                   = lightPropertiesBuffer.buffer;
	lightPropertiesBufferInfo.offset                   = (lightPropertiesBufferInfo.offset + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1);
	lightPropertiesBufferInfo.range                    = sizeof(LightProperties);

	descriptorWrites[1].dstSet          = descriptorSet;
	descriptorWrites[1].dstBinding      = 2;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo     = &lightPropertiesBufferInfo;

	if (!texturePath.empty())
	{
		texture.Load(devices, api.commandPool, api.deviceQueue.GetQueue(QueueType::GRAPHICS), texturePath.c_str());

		vk::DescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout             = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageInfo.imageView               = texture.imageView;
		imageInfo.sampler                 = texture.sampler;

		descriptorWrites[2].dstSet          = descriptorSet;
		descriptorWrites[2].dstBinding      = 1;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo      = &imageInfo;
	}
	else
	{
		// Create a 1x1 white texture
		std::vector<unsigned char> whitePixel = { 255, 255, 255, 255 };
		Texture                    whiteTex;

		// Finally create the texture
		whiteTex.Create(devices, api.commandPool, api.deviceQueue.GetQueue(QueueType::GRAPHICS), whitePixel.data(), 1, 1);
		texture = whiteTex;

		vk::DescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout             = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageInfo.imageView               = whiteTex.imageView;
		imageInfo.sampler                 = whiteTex.sampler;

		descriptorWrites[2].dstSet          = descriptorSet;
		descriptorWrites[2].dstBinding      = 1;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo      = &imageInfo;
	}

	devices.m_Device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void Mesh::UpdateMVPDescriptorSet(vk::Device& device)
{
	vk::DescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer                   = mvpMatrixBuffer.buffer;
	bufferInfo.offset                   = 0;
	bufferInfo.range                    = sizeof(ModelViewProjection);

	std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {};

	descriptorWrites[0].dstSet          = descriptorSet;
	descriptorWrites[0].dstBinding      = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo     = &bufferInfo;

	device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void Mesh::UpdateLightPropertiesDescriptorSet(vk::Device& device)
{
	lightPropertiesBufferInfo.buffer = lightPropertiesBuffer.buffer;
	lightPropertiesBufferInfo.offset = lightPropertiesBufferInfo.offset;
	lightPropertiesBufferInfo.range  = sizeof(LightProperties);

	std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {};

	descriptorWrites[0].dstSet          = descriptorSet;
	descriptorWrites[0].dstBinding      = 2; // assuming this is the correct binding for the light properties buffer
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo     = &lightPropertiesBufferInfo;

	device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void Mesh::UpdateMVPMatrix(vk::Device& device, ModelViewProjection mvp)
{
	this->mvpMatrix = mvp;

	void* data;
	device.mapMemory(mvpMatrixBuffer.memory, 0, sizeof(ModelViewProjection), {}, &data);
	memcpy(data, &mvp, sizeof(ModelViewProjection));
	device.unmapMemory(mvpMatrixBuffer.memory);

	UpdateMVPDescriptorSet(device);
}

void Mesh::UpdateLightProperties(vk::Device& device, LightProperties lightProperties)
{
	this->lightProperties = lightProperties;

	void* data;
	device.mapMemory(lightPropertiesBuffer.memory, 0, sizeof(lightProperties), {}, &data);
	memcpy(data, &lightProperties, sizeof(lightProperties));
	device.unmapMemory(lightPropertiesBuffer.memory);

	UpdateLightPropertiesDescriptorSet(device);
}

} // namespace vulkan
