#pragma once
#include <string>
// Just a name and id

namespace components
{

class Tag
{
public:
	Tag() = default;
	Tag(const std::string& name, unsigned int id) : m_Name(name), m_ID(id) {}
	~Tag() = default;

	// Getters
	const std::string& GetName() const { return m_Name; }
	unsigned int GetID() const { return m_ID; }

	// Setters
	void SetName(const std::string& name) { m_Name = name; }
	void SetID(unsigned int id) { m_ID = id; }

	std::string m_Name;
	unsigned int m_ID = 0;
};
} // namespace components