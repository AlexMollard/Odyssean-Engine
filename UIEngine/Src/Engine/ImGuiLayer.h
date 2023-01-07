#pragma once
// Wrappers and helper functions for imgui library
#include "BS_thread_pool.hpp"
#include "ECS.h"
#include "imgui.h"

struct GLFWwindow;

class ImGuiLayer
{
public:
	ImGuiLayer() = default;
	~ImGuiLayer();

	void Init(GLFWwindow* window);

	void SetStyle();
	void DisplaySystemStats();
	void ShowHierarchyWindow(bool* p_open);
	void ShowInspectorWindow(bool* p_open, flecs::entity entity);
	void ShowProfilerWindow(bool* p_open);
	void NewFrame(BS::thread_pool& pool); // before rendering (where you would call imgui::NewFrame)
	void UpdateViewPorts();

private:
	ImGuiIO* io;
};
