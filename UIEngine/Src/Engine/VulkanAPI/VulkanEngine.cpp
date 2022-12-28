#include "pch.h"

#include "VulkanEngine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

VulkanEngine::~VulkanEngine()
{
	Cleanup();
}

void VulkanEngine::Init(const char* windowName, int width, int height)
{
	m_close = false;
	std::cout << "Press ESC to exit.";

	try
	{
		InitWindow();
		InitVulkan();
	}
	catch (vk::SystemError& err)
	{
		std::cout << "vk::SystemError: " << err.what() << std::endl;
		exit(-1);
	}
	catch (std::exception& err)
	{
		std::cout << "std::exception: " << err.what() << std::endl;
		exit(-1);
	}
	catch (...)
	{
		std::cout << "unknown error\n";
		exit(-1);
	}
}

float VulkanEngine::Update()
{
	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		m_close = true;
		glfwSetWindowShouldClose(window, true);
	}

	return 1;
}

void VulkanEngine::Render() {}

void* VulkanEngine::GetWindow()
{
	return nullptr;
}

void VulkanEngine::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanEngine::InitVulkan()
{
	CreateInstance();
}

void VulkanEngine::Cleanup()
{
	glfwDestroyWindow(window);
	glfwTerminate();
	//m_Instance.destroy();
}

void VulkanEngine::CreateInstance()
{
	// initialize the vk::ApplicationInfo structure
	vk::ApplicationInfo applicationInfo("UI", 1, "UIengine", 1, VK_API_VERSION_1_1);

	// initialize the vk::InstanceCreateInfo
	vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo);

	// create an Instance
	//m_Instance = vk::createInstance(instanceCreateInfo);
}