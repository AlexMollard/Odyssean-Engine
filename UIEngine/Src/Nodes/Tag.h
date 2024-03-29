#pragma once

#include "Node.h"
#include <string>
// Just a name and id

namespace node
{
class Tag : Node
{
public:
	unsigned int m_ID = 0;
	char m_Name[256];

	Tag()  = default;
	~Tag() = default;

	// Getters
	unsigned int GetID() const
	{
		return m_ID;
	}
	char* GetName()
	{
		return m_Name;
	}

	// Setters
	void SetID(unsigned int id)
	{
		m_ID = id;
	}
	void SetName(const char* name)
	{
		strcpy_s(m_Name, name);
	}

	// Inspector
	void Inspector() override
	{
		if (!ImGui::CollapsingHeader("Tag"))
		{
			ImGui::InputText("Name", m_Name, 256);
			ImGui::DragInt("ID", (int*)&m_ID);
		}
	}
};
} // namespace node