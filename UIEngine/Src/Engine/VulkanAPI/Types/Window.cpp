#include "Window.h"

Window::~Window()
{}

void Window::Initialize(const char* windowName, int width, int height)
{
	m_Width  = width;
	m_Height = height;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_Window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
		auto app     = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->m_close = true;
	});

	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto app         = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->m_Keys[key] = action != GLFW_RELEASE;
	});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
		auto app                    = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->m_MouseButtons[button] = action != GLFW_RELEASE;
	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
		auto app      = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->m_MouseX = xpos;
		app->m_MouseY = ypos;
	});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
		auto app       = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->m_ScrollX = xoffset;
		app->m_ScrollY = yoffset;
	});

	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		auto app                  = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->m_FramebufferResized = true;
	});
}

void Window::Update()
{
	glfwPollEvents();

	if (m_FramebufferResized)
	{
		glfwGetFramebufferSize(m_Window, &m_Width, &m_Height);
		m_FramebufferResized = false;
	}

	m_close = glfwWindowShouldClose(m_Window);

	m_Keys[GLFW_KEY_ESCAPE] = glfwGetKey(m_Window, GLFW_KEY_ESCAPE);

	m_MouseButtons[GLFW_MOUSE_BUTTON_LEFT]   = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT);
	m_MouseButtons[GLFW_MOUSE_BUTTON_RIGHT]  = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT);
	m_MouseButtons[GLFW_MOUSE_BUTTON_MIDDLE] = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_MIDDLE);

	glfwGetCursorPos(m_Window, &m_MouseX, &m_MouseY);
	// Need to setup scroll stuff again

	if (m_Keys[GLFW_KEY_ESCAPE]) { m_close = true; }
}

void Window::Destroy()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

// GetRequiredExtensions
std::vector<const char*> Window::GetRequiredExtensions()
{
	uint32_t     glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	return extensions;
}

bool Window::GetMinimized()
{
	bool m_Minimized = false;
	if (m_Width == 0 || m_Height == 0) { m_Minimized = true; }
	return m_Minimized;
}

int Window::GetMouseState(bool mouseIndex)
{
	return m_MouseButtons[mouseIndex];
}

float Window::GetMouseWheel()
{
	return m_ScrollY;
}

void Window::ResetMouseWheel()
{
	m_ScrollY = 0;
}
