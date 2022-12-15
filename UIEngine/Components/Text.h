#pragma once
namespace components
{
class Text
{
public:
	Text() = default;
	Text(const std::string& text, const std::string& font, int size, const glm::vec4& color)
	: m_Text(text), m_Font(font), m_Size(size), m_Color(color)
	{}
	~Text() = default;

	// Getters
	const std::string& GetText() const { return m_Text; }
	const std::string& GetFont() const { return m_Font; }
	int GetSize() const { return m_Size; }
	const glm::vec4& GetColor() const { return m_Color; }

	// Setters
	void SetText(const std::string& text) { m_Text = text; }
	void SetFont(const std::string& font) { m_Font = font; }
	void SetSize(int size) { m_Size = size; }
	void SetColor(const glm::vec4& color) { m_Color = color; }
private:
	std::string m_Text;
	std::string m_Font;
	int m_Size;
	glm::vec4 m_Color;
};
} // namespace components
