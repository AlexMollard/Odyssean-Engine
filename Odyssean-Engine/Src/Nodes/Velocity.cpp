#include "Velocity.h"

#include "imgui.h"


void node::Velocity::Inspector()
{
	if (!ImGui::CollapsingHeader("Velocity"))
	{
		ImGui::DragFloat("X", &x);
		ImGui::DragFloat("Y", &y);
	}
}
