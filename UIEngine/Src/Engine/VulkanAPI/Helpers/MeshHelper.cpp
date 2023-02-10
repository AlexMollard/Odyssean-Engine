#include "pch.h"

#include "MeshHelper.h"

#include "../Types/Mesh.h"
#include "../Types/SubMesh.h"
#include "../Types/VkContainer.h"
#include "../Types/VulkanMaterial.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

std::vector<VulkanWrapper::SubMesh> MeshHelper::LoadModel(const std::string& filePath)
{
	// Load the model using assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals);

	// Check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
		return {};
	}

	// Process the scene and extract submeshes
	std::vector<VulkanWrapper::SubMesh> subMeshes;
	ProcessNode(scene, scene->mRootNode, subMeshes);

	return subMeshes;
}

void MeshHelper::CreateMesh(VulkanWrapper::VkContainer& api, const std::vector<VulkanWrapper::SubMesh>& subMeshes, VulkanWrapper::Mesh& mesh)
{
	mesh.SetSubMeshes(subMeshes);

	// Allocate buffers and descriptors for the mesh
	mesh.AllocateBuffers(api.deviceQueue);
	mesh.AllocateDescriptors(api);
}

void MeshHelper::DestroyMesh(vk::Device& device, VulkanWrapper::Mesh& mesh)
{
	// Free buffers and descriptors for the mesh
	mesh.FreeBuffers();
}

void MeshHelper::ProcessNode(const aiScene* scene, aiNode* node, std::vector<VulkanWrapper::SubMesh>& subMeshes)
{
	// Process all meshes in the node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(scene, mesh, subMeshes);
	}

	// Recursively process all children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(scene, node->mChildren[i], subMeshes);
	}
}

void MeshHelper::ProcessMesh(const aiScene* scene, aiMesh* mesh, std::vector<VulkanWrapper::SubMesh>& subMeshes)
{
	// Create a new submesh
	VulkanWrapper::SubMesh subMesh;

	// Process vertex positions, normals, and texture coordinates
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		VulkanWrapper::Vertex vertex{};

		// Process vertex positions
		vertex.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

		// Process vertex normals
		if (mesh->mNormals != nullptr)
		{
			vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		}

		// Process vertex texture coordinates
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
		else
		{
			vertex.texCoord = ComputeTexCoords(vertex.pos);
		}
		subMesh.m_vertices.push_back(vertex);

		// Process vertex tangents and bitangents
		if (mesh->mTangents != nullptr)
		{
			subMesh.m_tangents.emplace_back(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		}
		if (mesh->mBitangents != nullptr)
		{
			subMesh.m_bitangents.emplace_back(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		}

		// Process vertex colors
		if (mesh->mColors[0] != nullptr)
		{
			vertex.color = { mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a };
		}
		else
		{
			vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		}

		// Process vertex material
		if (mesh->mMaterialIndex >= 0)
		{
			VulkanWrapper::VulkanMaterial::ProcessMaterial(scene->mMaterials[mesh->mMaterialIndex], subMesh);
		}
	}

	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			subMesh.m_indices.push_back(face.mIndices[j]);
		}
	}

	// CalculateTangents(subMesh);

	subMeshes.push_back(subMesh);
}

glm::vec2 MeshHelper::ComputeTexCoords(glm::vec3 pos)
{
	// Compute spherical coordinates
	float theta = atan2(pos.z, pos.x);
	float phi   = acos(pos.y);

	// Compute texture coordinates
	float u = 1.0f - (theta + glm::pi<float>()) / (2.0f * glm::pi<float>());
	float v = (phi + glm::half_pi<float>()) / glm::pi<float>();

	return { u, v };
}

void MeshHelper::CalculateTangents(VulkanWrapper::SubMesh& subMesh)
{
	// Calculate tangents and bitangents
	for (size_t i = 0; i < subMesh.m_indices.size(); i += 3)
	{
		// Shortcuts for vertices
		VulkanWrapper::Vertex& v0 = subMesh.m_vertices[subMesh.m_indices[i + 0]];
		VulkanWrapper::Vertex& v1 = subMesh.m_vertices[subMesh.m_indices[i + 1]];
		VulkanWrapper::Vertex& v2 = subMesh.m_vertices[subMesh.m_indices[i + 2]];

		// Shortcuts for UVs
		glm::vec2& uv0 = v0.texCoord;
		glm::vec2& uv1 = v1.texCoord;
		glm::vec2& uv2 = v2.texCoord;

		// Edges of the triangle : postion delta
		glm::vec3 deltaPos1 = v1.pos - v0.pos;
		glm::vec3 deltaPos2 = v2.pos - v0.pos;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r             = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent   = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		// Set the same tangent for all three vertices of the triangle.
		// They will be merged later, in vboindexer.cpp
		v0.tangent = tangent;
		v1.tangent = tangent;
		v2.tangent = tangent;

		v0.bitangent = bitangent;
		v1.bitangent = bitangent;
		v2.bitangent = bitangent;

		subMesh.m_tangents.push_back(tangent);
		subMesh.m_bitangents.push_back(bitangent);
	}
}