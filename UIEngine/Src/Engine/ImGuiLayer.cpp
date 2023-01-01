#include "pch.h"

#include "ImGuiLayer.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "windows.h"
#include <GLFW/glfw3.h>

ImGuiLayer::~ImGuiLayer()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::Init(GLFWwindow* window)
{
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	(void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io->Fonts->AddFontFromFileTTF("../Resources/Fonts/FiraCode-Regular.ttf", 18);
	SetStyle();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiLayer::SetStyle()
{
	constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b) { return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f); };

	auto&   style  = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	const ImVec4 bgColor          = ColorFromBytes(37, 37, 38);
	const ImVec4 lightBgColor     = ColorFromBytes(82, 82, 85);
	const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

	const ImVec4 panelColor       = ColorFromBytes(51, 51, 55);
	const ImVec4 panelHoverColor  = ColorFromBytes(29, 151, 236);
	const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

	const ImVec4 textColor         = ColorFromBytes(255, 255, 255);
	const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
	const ImVec4 borderColor       = ColorFromBytes(78, 78, 78);

	colors[ImGuiCol_Text]                 = textColor;
	colors[ImGuiCol_TextDisabled]         = textDisabledColor;
	colors[ImGuiCol_TextSelectedBg]       = panelActiveColor;
	colors[ImGuiCol_WindowBg]             = bgColor;
	colors[ImGuiCol_ChildBg]              = bgColor;
	colors[ImGuiCol_PopupBg]              = bgColor;
	colors[ImGuiCol_Border]               = borderColor;
	colors[ImGuiCol_BorderShadow]         = borderColor;
	colors[ImGuiCol_FrameBg]              = panelColor;
	colors[ImGuiCol_FrameBgHovered]       = panelHoverColor;
	colors[ImGuiCol_FrameBgActive]        = panelActiveColor;
	colors[ImGuiCol_TitleBg]              = bgColor;
	colors[ImGuiCol_TitleBgActive]        = bgColor;
	colors[ImGuiCol_TitleBgCollapsed]     = bgColor;
	colors[ImGuiCol_MenuBarBg]            = panelColor;
	colors[ImGuiCol_ScrollbarBg]          = panelColor;
	colors[ImGuiCol_ScrollbarGrab]        = lightBgColor;
	colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
	colors[ImGuiCol_ScrollbarGrabActive]  = veryLightBgColor;
	colors[ImGuiCol_CheckMark]            = panelActiveColor;
	colors[ImGuiCol_SliderGrab]           = panelHoverColor;
	colors[ImGuiCol_SliderGrabActive]     = panelActiveColor;
	colors[ImGuiCol_Button]               = panelColor;
	colors[ImGuiCol_ButtonHovered]        = panelHoverColor;
	colors[ImGuiCol_ButtonActive]         = panelHoverColor;
	colors[ImGuiCol_Header]               = panelColor;
	colors[ImGuiCol_HeaderHovered]        = panelHoverColor;
	colors[ImGuiCol_HeaderActive]         = panelActiveColor;
	colors[ImGuiCol_Separator]            = borderColor;
	colors[ImGuiCol_SeparatorHovered]     = borderColor;
	colors[ImGuiCol_SeparatorActive]      = borderColor;
	colors[ImGuiCol_ResizeGrip]           = bgColor;
	colors[ImGuiCol_ResizeGripHovered]    = panelColor;
	colors[ImGuiCol_ResizeGripActive]     = lightBgColor;
	colors[ImGuiCol_PlotLines]            = panelActiveColor;
	colors[ImGuiCol_PlotLinesHovered]     = panelHoverColor;
	colors[ImGuiCol_PlotHistogram]        = panelActiveColor;
	colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
	colors[ImGuiCol_DragDropTarget]       = bgColor;
	colors[ImGuiCol_NavHighlight]         = bgColor;
	colors[ImGuiCol_DockingPreview]       = panelActiveColor;
	colors[ImGuiCol_Tab]                  = bgColor;
	colors[ImGuiCol_TabActive]            = panelActiveColor;
	colors[ImGuiCol_TabUnfocused]         = bgColor;
	colors[ImGuiCol_TabUnfocusedActive]   = panelActiveColor;
	colors[ImGuiCol_TabHovered]           = panelHoverColor;

	style.WindowRounding    = 0.0f;
	style.ChildRounding     = 0.0f;
	style.FrameRounding     = 0.0f;
	style.GrabRounding      = 0.0f;
	style.PopupRounding     = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.TabRounding       = 0.0f;
}

void ImGuiLayer::DisplaySystemStats()
{
	ImGui::Text("CPU Usage: %f", 0.0f);
	ImGui::Text("GPU Usage: %f", 0.0f);
	ImGui::Text("GPU Memory: %f", 0.0f);
}

void ImGuiLayer::ShowProfilerWindow(bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Profiler", p_open))
	{
		ImGui::End();
		return;
	}

	float frame_rate = ImGui::GetIO().Framerate;

	// Frame rate
	static float ms_per_frame[120]  = { 0 };
	static float m_min              = -1.0f;
	static float m_max              = FLT_MAX;
	static int   ms_per_frame_idx   = 0;
	static float ms_per_frame_accum = 0.0f;

	static bool m_paused = false;
	if (!m_paused)
	{
		ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
		ms_per_frame[ms_per_frame_idx] = 1000.0f / frame_rate;
		ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
		ms_per_frame_idx = (ms_per_frame_idx + 1) % IM_ARRAYSIZE(ms_per_frame);

		float range = m_max - m_min;

		// Update the min and max if the current frame rate is lower or higher than the current min and max
		m_min = std::min(m_min, ms_per_frame[ms_per_frame_idx]);
		m_max = std::max(m_max, ms_per_frame[ms_per_frame_idx]);

	}

	// Stretch the plot to the full width of the window
	ImGui::PushItemWidth(-1);
	// Plot the frame rate over time
	ImGui::PlotLines("Frame Rate", ms_per_frame, IM_ARRAYSIZE(ms_per_frame), ms_per_frame_idx, NULL, m_min, m_max, ImVec2(0, 80));
	ImGui::PopItemWidth();
	// The above but with colors
	ImVec4 highAvgColor = ImVec4(0.1f, 1.0f, 0.1f, 1.0f);
	ImVec4 lowAvgColor  = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
	float  avgFrameRate = ms_per_frame_accum / IM_ARRAYSIZE(ms_per_frame);

	ImGui::Text("Average Frame Rate: ");
	ImGui::SameLine();
	// Blend between the high and low colors based on the average frame rate
	ImGui::PushStyleColor(ImGuiCol_Text,
						  ImVec4(ImLerp(highAvgColor.x, lowAvgColor.x, avgFrameRate / 16.666f), ImLerp(highAvgColor.y, lowAvgColor.y, avgFrameRate / 16.666f), ImLerp(highAvgColor.z, lowAvgColor.z, avgFrameRate / 16.666f), 1.0f));
	ImGui::Text("%.3f ms/frame", avgFrameRate);
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.1f, 1.0f));
	ImGui::Text("(%0.1f FPS)", 1000.0f / avgFrameRate);
	ImGui::PopStyleColor();

	// Button to pause (Green) or resume (Red) the profiler
	if (m_paused)
	{
		// Color the button green
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.9f, 0.1f, 1.0f));
	}
	else
	{
		// Color the button red
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
	}

	// Button to pause or resume the profiler (Right aligned below the plots)
	ImGui::SameLine(ImGui::GetWindowWidth() - 80);

	if (ImGui::Button(m_paused ? "Resume" : "Pause")) { m_paused = !m_paused; }

	ImGui::PopStyleColor(2);

	// Display system stats
	//DisplaySystemStats();

	ImGui::End();
}

void ImGuiLayer::NewFrame(BS::thread_pool& pool)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	auto dockspaceFn = [&]() {
		// Make the entire window be a dockspace
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(2);
		ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();

		// Menu Bar
		if (ImGui::BeginMainMenuBar())
		{
			static bool showProfiler     = true;
			static bool showImGuiMetrics = false;
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N")) {}
				if (ImGui::MenuItem("Open...", "Ctrl+O")) {}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {}
				if (ImGui::MenuItem("Save As...", "")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
				if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {} // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
				if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
				if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Profiler")) showProfiler = !showProfiler;
				if (ImGui::MenuItem("Show Imgui Metrics")) showImGuiMetrics = !showImGuiMetrics;
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();

			if (showProfiler) { ShowProfilerWindow(&showProfiler); }

			if (showImGuiMetrics) { ImGui::ShowMetricsWindow(&showImGuiMetrics); }
		}
	};
	pool.submit(dockspaceFn);
}

void ImGuiLayer::UpdateViewPorts()
{
	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}