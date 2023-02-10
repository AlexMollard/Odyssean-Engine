#pragma once
// Wrappers and helper functions for imgui library
#include "BS_thread_pool.hpp"
#include "ECS.h"
#include "imgui.h"

struct GLFWwindow;

class ImGuiLayer
{
public:
	ImGuiLayer()  = default;
	~ImGuiLayer() = default;

	void Init(GLFWwindow* window);

	static void SetStyle();
	void DisplaySystemStats() const;
	void ShowHierarchyWindow(bool* p_open) const;
	void ShowInspectorWindow(bool* p_open, flecs::entity entity) const;
	void ShowProfilerWindow(bool* p_open) const;
	void NewFrame(BS::thread_pool& pool); // before rendering (where you would call imgui::NewFrame)
	void UpdateViewPorts() const;

private:
	ImGuiIO* io;
};
