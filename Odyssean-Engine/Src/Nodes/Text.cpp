#include "Text.h"

#include "imgui.h"

void node::Text::Inspector()
{
	if (!ImGui::CollapsingHeader("Text"))
	{
		ImGui::InputText("Text", &m_Text[0], m_Text.size());
		ImGui::InputText("Font", &m_Font[0], m_Font.size());
		ImGui::DragFloat("Scale", &m_Scale);
		ImGui::ColorEdit4("Color", &m_Color[0]);
	}
}
