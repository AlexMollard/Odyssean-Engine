#pragma once

#include "Node.h"
#include "glm/glm.hpp"
#include <string>

namespace node
{
class Text : Node
{
public:
	Text() = default;
	Text(const std::string& text, const std::string& font, float scale, const glm::vec4& color) : m_Text(text), m_Font(font), m_Scale(scale), m_Color(color) {}
	~Text() = default;

	// Getters
	const std::string& GetText() const
	{
		return m_Text;
	}
	const std::string& GetFont() const
	{
		return m_Font;
	}
	float GetScale() const
	{
		return m_Scale;
	}
	const glm::vec4& GetColor() const
	{
		return m_Color;
	}
	const glm::vec3 GetPosition()
	{
		return m_Position;
	}

	// Setters
	void SetText(const std::string& text)
	{
		m_Text = text;
	}
	void SetFont(const std::string& font)
	{
		m_Font = font;
	}
	void SetSize(float scale)
	{
		m_Scale = scale;
	}
	void SetColor(const glm::vec4& color)
	{
		m_Color = color;
	}
	void SetPosition(const glm::vec3& position)
	{
		m_Position = position;
	}

	// Inspector
	void Inspector() override;

	std::string m_Text;
	std::string m_Font;
	float m_Scale        = 1.0f;
	glm::vec4 m_Color    = glm::vec4(1.0f);
	glm::vec3 m_Position = glm::vec3(0.0f);
};
} // namespace node
