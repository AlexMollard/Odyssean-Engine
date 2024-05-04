#include "Tag.h"

#include "imgui.h"

void node::Tag::Inspector()
{
	if (!ImGui::CollapsingHeader("Tag"))
	{
		ImGui::InputText("Name", m_Name, 256);
		ImGui::DragInt("ID", (int*)&m_ID);
	}
}
