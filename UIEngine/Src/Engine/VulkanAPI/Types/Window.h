#pragma once
// This is strictly for the vulkan API

//glfw
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
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
};
