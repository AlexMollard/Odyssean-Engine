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
	Assimp::Importer importer;
	const unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals;
	const aiScene* scene     = importer.ReadFile(filePath, flags);

	// Check for errors in the entire scene
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
		return {};
	}

	// Process each mesh in the scene
	std::vector<VulkanWrapper::SubMesh> subMeshes;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		VulkanWrapper::SubMesh subMesh;
		if (ProcessMesh(mesh, subMesh))
		{
			const auto material = scene->mMaterials[mesh->mMaterialIndex];
			if (material)
				VulkanWrapper::VulkanMaterial::ProcessMaterial(material, subMesh);

			subMeshes.emplace_back(std::move(subMesh));
		}
	}

	return subMeshes;
}

void MeshHelper::CreateMesh(VulkanWrapper::VkContainer& api, const std::vector<VulkanWrapper::SubMesh>& subMeshes, VulkanWrapper::Mesh& mesh)
{
	mesh.SetSubMeshes(subMeshes);

	// Allocate buffers and descriptors for the mesh
	mesh.AllocateBuffers(api.deviceQueue);
	mesh.AllocateDescriptors(api);
}

void MeshHelper::DestroyMesh(VulkanWrapper::Mesh& mesh)
{
	// Free buffers and descriptors for the mesh
	mesh.FreeBuffers();
}

bool MeshHelper::ProcessMesh(const aiMesh* mesh, VulkanWrapper::SubMesh& subMesh)
{
	if (mesh->mNumVertices == 0)
	{
		return false;
	}

	const bool hasNormals   = mesh->HasNormals();
	const bool hasTexCoords = mesh->HasTextureCoords(0);
	const bool hasTangents  = mesh->HasTangentsAndBitangents();
	const bool hasColors    = mesh->HasVertexColors(0);

	// Process vertices
	const auto numVertices = mesh->mNumVertices;
	subMesh.m_vertices.resize(numVertices);
	for (unsigned int i = 0; i < numVertices; i++)
	{
		VulkanWrapper::Vertex& vertex = subMesh.m_vertices[i];

		// Process vertex positions
		vertex.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

		// Process vertex normals
		if (hasNormals)
		{
			vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		}

		// Process vertex texture coordinates
		if (hasTexCoords)
		{
			vertex.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
		else
		{
			vertex.texCoord = ComputeTexCoords(vertex.pos);
		}

		// Process vertex tangents and bitangents
		if (hasTangents)
		{
			subMesh.m_tangents.emplace_back(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			subMesh.m_bitangents.emplace_back(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
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

	// Process vertex colors
	if (hasColors)
	{
		for (unsigned int i = 0; i < numVertices; i++)
		{
			VulkanWrapper::Vertex& vertex = subMesh.m_vertices[i];
			vertex.color                  = { mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a };
		}
	}
	else
	{
		for (unsigned int i = 0; i < numVertices; i++)
		{
			VulkanWrapper::Vertex& vertex = subMesh.m_vertices[i];
			vertex.color                  = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}

	return true;
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