#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "VkMesh.h"

struct RenderData;
struct Init;

class VulkanRenderer
{
public:
	static RenderData SetupRenderData(Init& init);

	static int CreateSwapchain(Init& init);
	static int RecreateSwapchain(Init& init, RenderData& data);
	
	static int GetQueues(Init& init, RenderData& data);
	
	static int CreateRenderPass(Init& init, RenderData& data);
	static int SetUpPipelineLayout(Init& init, RenderData& renderData);
	static int CreateGraphicsPipeline(Init& init, RenderData& data);
	static int CreateFramebuffers(Init& init, RenderData& data);
	static int CreateCommandPool(Init& init, RenderData& data);
	static int CreateCommandBuffers(Init& init, RenderData& data);
	static int CreateSyncObjects(Init& init, RenderData& data);

	// Mesh Loading (Mesh has AllocatedBuffer and vector of vertices)
	static vulkan::Mesh LoadModel(const char* path);
	static int SetUpMeshBuffers(Init& init, RenderData& renderData, vulkan::Mesh& mesh);
	static int SetUpTexture(Init& init, RenderData& renderData, vulkan::Mesh& mesh);
	static int SetUpMeshDescriptorInfo(Init& init, vulkan::Mesh& mesh);
	static int SetUpMeshDescriptorSets(Init& init, vulkan::Mesh& mesh);
	static int RecordCommandBuffers(Init& init, RenderData& renderData, vulkan::Mesh& mesh);
	static int AddMesh(vulkan::Mesh& mesh);

	// Draw Frame
	static int DrawFrame(Init& init, RenderData& data);

	// Clean Up
	static int CleanUp(Init& init, RenderData& data);

private:
	// Singleton for storing all the meshes loaded
	static std::vector<vulkan::Mesh> m_Meshes;
};
