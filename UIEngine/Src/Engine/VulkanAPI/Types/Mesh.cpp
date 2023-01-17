#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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
	device.freeMemory(vertexBuffer.memory);
	device.destroyBuffer(vertexBuffer.buffer);
	device.freeMemory(indexBuffer.memory);
	device.destroyBuffer(indexBuffer.buffer);
	device.freeMemory(uboMatrixBuffer.memory);
	device.destroyBuffer(uboMatrixBuffer.buffer);
	device.destroyDescriptorPool(descriptorPool);
	device.destroyDescriptorSetLayout(descriptorSetLayout);
	// texture.Destroy(device);
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
	CreateUBOMatrixBuffer(api.deviceQueue, api.commandPool, glm::mat4(1.0f));
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

void Mesh::CreateUBOMatrixBuffer(DeviceQueue& devices, vk::CommandPool& commandPool, glm::mat4 uboMatrix)
{
	vk::Result res = devices.CreateBuffer(vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uboMatrixBuffer, &uboMatrix, sizeof(uboMatrix));
	if (res != vk::Result::eSuccess) { throw std::runtime_error("failed to create model matrix buffer!"); }

	this->uboMatrix = uboMatrix;
}

void Mesh::CreateLightPropertiesBuffer(DeviceQueue& devices, vk::CommandPool& commandPool, const LightProperties& lightProperties)
{
	vk::Result res =
		devices.CreateBuffer(vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, lightPropertiesBuffer, &lightProperties, sizeof(lightProperties));
	if (res != vk::Result::eSuccess) { throw std::runtime_error("failed to create light properties buffer!"); }

	this->lightProperties = lightProperties;
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
	poolInfo.maxSets                                = 2;

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
	bufferInfo.buffer                   = uboMatrixBuffer.buffer;
	bufferInfo.offset                   = 0;
	bufferInfo.range                    = sizeof(glm::mat4);

	std::array<vk::WriteDescriptorSet, 3> descriptorWrites = {};

	descriptorWrites[0].dstSet          = descriptorSet;
	descriptorWrites[0].dstBinding      = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo     = &bufferInfo;

	if (!texturePath.empty())
	{
		vk::DescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout             = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageInfo.imageView               = texture.imageView;
		imageInfo.sampler                 = texture.sampler;

		descriptorWrites[1].dstSet          = descriptorSet;
		descriptorWrites[1].dstBinding      = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo      = &imageInfo;
	}
	else
	{
		// Create a 1x1 white texture
		std::vector<unsigned char> whitePixel = { 255, 255, 255, 255 };
		Texture whiteTex;

		// Finally create the texture
		whiteTex.Create(devices, api.commandPool, api.deviceQueue.GetQueue(QueueType::GRAPHICS), whitePixel.data(), 1, 1);
		texture = whiteTex;

		vk::DescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout             = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageInfo.imageView               = whiteTex.imageView;
		imageInfo.sampler                 = whiteTex.sampler;

		descriptorWrites[1].dstSet          = descriptorSet;
		descriptorWrites[1].dstBinding      = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo      = &imageInfo;
	}

	// Light properties
	vk::DescriptorBufferInfo lightPropertiesBufferInfo = {};
	lightPropertiesBufferInfo.buffer                   = lightPropertiesBuffer.buffer;
	lightPropertiesBufferInfo.offset                   = 0;
	lightPropertiesBufferInfo.range                    = sizeof(LightProperties);

	descriptorWrites[2].dstSet          = descriptorSet;
	descriptorWrites[2].dstBinding      = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pBufferInfo     = &lightPropertiesBufferInfo;

	devices.m_Device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void Mesh::UpdateUBODescriptorSet(vk::Device& device, vk::PipelineLayout& graphicsPipelineLayout)
{
	vk::DescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer                   = uboMatrixBuffer.buffer;
	bufferInfo.offset                   = 0;
	bufferInfo.range                    = sizeof(glm::mat4);

	std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {};

	descriptorWrites[0].dstSet          = descriptorSet;
	descriptorWrites[0].dstBinding      = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo     = &bufferInfo;

	device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);

	// Update model matrix buffer
	void* data;
	device.mapMemory(uboMatrixBuffer.memory, 0, sizeof(glm::mat4), {}, &data);
	memcpy(data, &uboMatrix, sizeof(glm::mat4));
	device.unmapMemory(uboMatrixBuffer.memory);
}

void Mesh::UpdateLightPropertiesDescriptorSet(vk::Device& device, vk::PipelineLayout& graphicsPipelineLayout)
{
	vk::DescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer                   = lightPropertiesBuffer.buffer;
	bufferInfo.offset                   = 0;
	bufferInfo.range                    = sizeof(LightProperties);

	std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {};

	descriptorWrites[0].dstSet          = descriptorSet;
	descriptorWrites[0].dstBinding      = 2;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo     = &bufferInfo;

	device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);

	// Update light properties buffer
	void* data;
	device.mapMemory(lightPropertiesBuffer.memory, 0, sizeof(LightProperties), {}, &data);
	memcpy(data, &lightProperties, sizeof(LightProperties));
	device.unmapMemory(lightPropertiesBuffer.memory);
}

void Mesh::UpdateUBOMatrix(vk::Device& device, vk::PipelineLayout& graphicsPipelineLayout, glm::mat4 uboMatrix)
{
	this->uboMatrix = uboMatrix;
	UpdateUBODescriptorSet(device, graphicsPipelineLayout);
}

void Mesh::UpdateLightProperties(vk::Device& device, vk::PipelineLayout& graphicsPipelineLayout, const LightProperties& lightProperties)
{
	this->lightProperties = lightProperties;
	UpdateLightPropertiesDescriptorSet(device, graphicsPipelineLayout);
}

} // namespace vulkan
