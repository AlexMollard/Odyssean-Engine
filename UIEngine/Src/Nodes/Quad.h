#pragma once

#include "Node.h"
#include "glm/glm.hpp"
namespace node
{
class Quad : Node
{
public:
	Quad() = default;
	Quad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) : m_Size(size), m_Color(color) {}
	~Quad() = default;

	// Getters
	const glm::vec2& GetSize() const
	{
		return m_Size;
	}
	const glm::vec4& GetColor() const
	{
		return m_Color;
	}
	const glm::vec2& GetAnchorPoint() const
	{
		return m_AnchorPoint;
	}

	// Setters
	void SetSize(const glm::vec2& size)
	{
		m_Size = size;
	}
	void SetColor(const glm::vec4& color)
	{
		m_Color = color;
	}
	void SetAnchorPoint(const glm::vec2 anchorPoint)
	{
		m_AnchorPoint = anchorPoint;
	}
	void SetTextureID(unsigned int id)
	{
		m_TextureID = id;
	}

	const unsigned int GetTextureID() const
	{
		return m_TextureID;
	}

	// Inspector
	void Inspector() override
	{
		if (!ImGui::CollapsingHeader("Quad"))
		{
			ImGui::ColorEdit4("Color", &m_Color[0]);
			ImGui::DragFloat2("Size", &m_Size[0]);
			ImGui::DragFloat2("Anchor Point", &m_AnchorPoint[0], 0.001f);
		}
	}

	glm::vec2 m_Size = glm::vec2(10.0f);
	glm::vec4 m_Color;
	glm::vec2 m_AnchorPoint = glm::vec2(0.0f, 0.0f);

	unsigned int m_TextureID = 0;
};
} // namespace node