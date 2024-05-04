#include "Transform.h"

#include "imgui.h"


void node::Transform::Inspector()
{
	if (!ImGui::CollapsingHeader("Transform"))
	{
		ImGui::DragFloat2("Position", &m_Position.x);
		ImGui::DragFloat("Rotation", &m_Rotation, 0.01f);
		ImGui::DragFloat2("Scale", &m_Scale.x);
	}
}
