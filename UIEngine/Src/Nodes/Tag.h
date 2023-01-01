#pragma once

#include "Node.h"
#include <string>
// Just a name and id

namespace node
{
class Tag : Node
{
	unsigned int m_ID = 0;
	char         m_Name[256];

public:
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
};
} // namespace node