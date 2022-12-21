#pragma once
// Wrappers and helper functions for imgui library
#include <GLFW/glfw3.h>
#include "imgui.h"
class ImGuiLayer
{
public:
	ImGuiLayer(GLFWwindow* window);
	~ImGuiLayer();
	
	void SetStyle();
	void NewFrame(); // before rendering (where you would call imgui::NewFrame)
	void UpdateViewPorts();

private:
	ImGuiIO* io;
};
