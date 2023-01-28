#pragma once
// This is strictly for the vulkan API

//glfw
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include <vulkan/vulkan.hpp>
#include <vector>
class Window
{
public:
	Window() = default;
	~Window();

	void Initialize(const char* windowName, int width, int height);
	void Update();
	void Destroy();

	bool GetClose() const
	{
		return m_close;
	}
	GLFWwindow* GetWindow() const
	{
		return m_Window;
	}

	// Gets the required extensions for glfw
	static std::vector<const char*> GetRequiredExtensions();

	vk::SurfaceKHR                    m_Surface;
	vk::SurfaceCapabilitiesKHR        m_SurfaceCapabilities;
	vk::Extent2D                      m_SurfaceExtent;
	vk::SurfaceFormatKHR              m_SurfaceFormat;
	vk::PresentModeKHR                m_SurfacePresentMode;
	size_t                            m_SurfaceImageCount;
	std::vector<vk::SurfaceFormatKHR> m_SurfaceFormats;
	std::vector<vk::PresentModeKHR>   m_SurfacePresentModes;

private:
	// Window
	int m_Width;
	int m_Height;

	GLFWwindow* m_Window;
	bool        m_close = false;

	// keys
	bool m_Keys[GLFW_KEY_LAST] = { false };

	// mouse buttons
	bool m_MouseButtons[GLFW_MOUSE_BUTTON_LAST] = { false };

	// mouse
	double m_MouseX = true;
	double m_MouseY = true;

	// scroll
	double m_ScrollX = true;
	double m_ScrollY = true;

	// Framebuffer Resize
	bool m_FramebufferResized = false;

public:
	bool GetMinimized();
	int GetMouseState(bool mouseIndex);
	float GetMouseWheel();
	void  ResetMouseWheel();
};
