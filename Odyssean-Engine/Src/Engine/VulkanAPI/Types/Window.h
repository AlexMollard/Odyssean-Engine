#pragma once
// This is strictly for the vulkan API

#include <vector>
#include <vulkan/vulkan.hpp>

class GLFWwindow;

class Window
{
public:
	Window()  = default;
	~Window() = default;

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

	vk::SurfaceKHR m_Surface;
	vk::SurfaceCapabilitiesKHR m_SurfaceCapabilities;
	vk::Extent2D m_SurfaceExtent;
	vk::SurfaceFormatKHR m_SurfaceFormat;
	vk::PresentModeKHR m_SurfacePresentMode;
	size_t m_SurfaceImageCount;
	std::vector<vk::SurfaceFormatKHR> m_SurfaceFormats;
	std::vector<vk::PresentModeKHR> m_SurfacePresentModes;

private:
	// Window
	int m_Width;
	int m_Height;

	GLFWwindow* m_Window;
	bool m_close = false;

	// keys
	bool m_Keys[348] = { false };

	// mouse buttons
	bool m_MouseButtons[7] = { false };

	// mouse
	double m_MouseX = true;
	double m_MouseY = true;

	// scroll
	double m_ScrollX = true;
	double m_ScrollY = true;

	// Framebuffer Resize
	bool m_FramebufferResized = false;

	// Timing stuff
	float m_Dt            = 0.0f;
	double m_LastFrame    = 0.0f;
	double m_CurrentFrame = 0.0f;

public:
	bool GetMinimized() const;
	int GetMouseState(bool mouseIndex) const;
	double GetMouseWheel() const;
	void ResetMouseWheel();
	float GetDt() const;
};
