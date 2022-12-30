#pragma once
#include "VkBootstrap.h"

struct Init;
class ShaderVulkan
{
public:
	static VkShaderModule CreateShaderModule(Init& init, const std::string& filename);

private:
	static const std::vector<char> ReadFile(const std::string& filename);
};
