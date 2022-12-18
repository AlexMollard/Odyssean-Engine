#pragma once
namespace components
{
class Quad
{
public:
	Quad() = default;
	Quad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) : m_Position(position), m_Size(size), m_Color(color) {}
	~Quad() = default;

	// Getters
	const glm::vec2& GetPosition() const { return m_Position; }
	const glm::vec2& GetSize() const { return m_Size; }
	const glm::vec4& GetColor() const { return m_Color; }
	const glm::vec2& GetAnchorPoint() const { return m_AnchorPoint; }

	// Setters
	void SetPosition(const glm::vec2& position) { m_Position = position; }
	void SetSize(const glm::vec2& size) { m_Size = size; }
	void SetColor(const glm::vec4& color) { m_Color = color; }
	void SetAnchorPoint(const glm::vec2 anchorPoint) { m_AnchorPoint = anchorPoint; }
	void SetTextureID(unsigned int id) { m_TextureID = id; }

	// AABB stuff
	const glm::vec2 GetTopLeft() const { return m_Position - m_Size / 2.0f; }
	const glm::vec2 GetTopRight() const { return m_Position + glm::vec2(m_Size.x, -m_Size.y) / 2.0f; }
	const glm::vec2 GetBottomLeft() const { return m_Position + glm::vec2(-m_Size.x, m_Size.y) / 2.0f; }
	const glm::vec2 GetBottomRight() const { return m_Position + m_Size / 2.0f; }
	const unsigned int GetTextureID() const { return m_TextureID; }

private:
	glm::vec2 m_Position;
	glm::vec2 m_Size;
	glm::vec4 m_Color;
	glm::vec2 m_AnchorPoint = glm::vec2(0.0f, 0.0f);
	
	unsigned int m_TextureID = 0;
};
} // namespace components