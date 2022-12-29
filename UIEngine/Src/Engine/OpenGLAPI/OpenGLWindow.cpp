#include "pch.h"

#include "OpenGLWindow.h"

#include "Tracy.hpp"
#include "TracyOpenGL.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

OpenGLWindow::~OpenGLWindow()
{
	Window_destroy();
}

void OpenGLWindow::Initialise(int width, int height, std::string_view name)
{
	if (!Window_intit(width, height, name))
	{
		std::cout << "Failed to load window" << std::endl;
	}
}

int OpenGLWindow::Window_intit(int width, int height, std::string_view name)
{
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(1); // V-Sync

	// Initializing Glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Glew is not having a good time" << std::endl;
	}

	// Outputting OpenGL Version and build
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	TracyGpuContext;

	return 1;
}

void OpenGLWindow::Update_Window()
{
	ZoneScopedN("window Update");

	{
		ZoneScopedN("SwapBuffers");
		glfwSwapBuffers(window);
	}

	{
		ZoneScopedN("glfwPollEvents");
		glfwPollEvents();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	now   = glfwGetTime();
	delta = (float)(now - last);
	last  = now;
}

int OpenGLWindow::Window_shouldClose()
{
	return glfwWindowShouldClose(window);
}

void OpenGLWindow::Window_destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

float OpenGLWindow::GetDeltaTime() const
{
	return delta;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}