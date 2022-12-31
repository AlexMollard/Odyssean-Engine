#include "ShaderVulkan.h"
#include "VulkanInit.h"
#include <fstream>

VkShaderModule ShaderVulkan::CreateShaderModule(Init& init, const std::string& filename)
{

	const std::vector<char>& code        = ShaderVulkan::ReadFile(filename);
	
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize                 = code.size();
	create_info.pCode                    = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (init->vkCreateShaderModule(init.device, &create_info, nullptr, &shaderModule) != VK_SUCCESS)
	{
		return VK_NULL_HANDLE; // failed to create shader module
	}

	return shaderModule;
}

const std::vector<char> ShaderVulkan::ReadFile(const std::string& filename) 
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	// check if file is open, this will also check if the file exists so make sure the dir exists
	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	size_t file_size = (size_t)file.tellg();
	std::vector<char> buffer(file_size);

	file.seekg(0);
	file.read(buffer.data(), static_cast<std::streamsize>(file_size));

	file.close();

	return buffer;
}
