#include "Quad.h"

#include "imgui.h"

void node::Quad::Inspector()
{
	if (!ImGui::CollapsingHeader("Quad"))
	{
		ImGui::ColorEdit4("Color", &m_Color[0]);
		ImGui::DragFloat2("Size", &m_Size[0]);
		ImGui::DragFloat2("Anchor Point", &m_AnchorPoint[0], 0.001f);
	}
}
