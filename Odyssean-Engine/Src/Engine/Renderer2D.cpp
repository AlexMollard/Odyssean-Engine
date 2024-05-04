#include "pch.h"

#include "Renderer2D.h"
#include <GL/glew.h>

#include <freetype/freetype.h>

#include "OpenGLAPI/OpenGLWindow.h"
#include "Texture.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <unordered_map>

#include "Engine/OpenGLAPI/ShaderOpenGL.h"

static constexpr size_t maxQuadCount   = 2000;
static constexpr size_t maxVertexCount = maxQuadCount * 4;
static constexpr size_t maxIndexCount  = maxQuadCount * 6;
static constexpr size_t maxTextures    = 31;

static unsigned int m_TextVAO;
static unsigned int m_TextVBO;

static ShaderOpenGL* m_TextShader = nullptr;

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoords;
	float texIndex;
};

struct RendererData
{
	GLuint quadVA = 0;
	GLuint quadVB = 0;
	GLuint quadIB = 0;

	GLuint whiteTexture       = 0;
	uint32_t whiteTextureSlot = 0;

	uint32_t indexCount = 0;

	Vertex* quadBuffer    = nullptr;
	Vertex* quadBufferPtr = nullptr;

	std::array<uint32_t, maxTextures> textureSlots = {};
	uint32_t textureSlotIndex                      = 1;
};

struct Character
{
	unsigned int texture;
	glm::vec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

// Static class to hold all the characters
class CharacterManager
{
public:
	static CharacterManager& Instance()
	{
		static CharacterManager instance;
		return instance;
	}

	CharacterManager(const CharacterManager&) = delete;
	void operator=(const CharacterManager&)   = delete;

	void AddCharacter(char c, const Character& character)
	{
		m_characters.try_emplace(c, character);
	}

	Character GetCharacter(char c) const
	{
		auto it = m_characters.find(c);
		S_ASSERT(it != m_characters.end(), "Character not found in the map");
		return it->second;
	}

private:
	CharacterManager() = default;
	std::unordered_map<char, Character> m_characters;
};

static RendererData data;

static const std::array<glm::vec2, 4> basicUVS = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };

Renderer2D::~Renderer2D()
{
	ShutDown();
	for (auto& texture : texturePool)
	{
		delete texture;
		texture = nullptr;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}
}

void Renderer2D::Draw()
{
	ShaderOpenGL::Use(*m_BasicShader);
	const float width  = OpenGLWindow::Instance().m_Width;
	const float height = OpenGLWindow::Instance().m_Height;

	// Setup the projection matrix to be orthographic with 0 being bottom left
	glm::mat4 proj = glm::ortho(0.0f, width, 0.0f, height, -100.0f, 100.0f);

	glm::mat4 model = glm::mat4(1.0f);

	// Adjust the view matrix to be centered so that 0,0 is the bottom left of the screen
	glm::mat4 view = glm::mat4(1.0f);

	// create the ubo
	unsigned int ubo;

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

	// update the ubo
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));

	// bind the ubo to the shader
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

	// Test to make sure the ubo is working
	ShaderOpenGL::setMat4(*m_BasicShader, "model", model);
	ShaderOpenGL::setMat4(*m_BasicShader, "view", view);
	ShaderOpenGL::setMat4(*m_BasicShader, "proj", proj);

	EndBatch();
	Flush();
	BeginBatch();
}

void Renderer2D::DrawQuad(glm::vec2 inPosition, glm::vec2 size, glm::vec4 color, glm::vec2 anchorPoint, float rotation, const unsigned int texId)
{
	if (data.indexCount >= maxIndexCount)
	{
		EndBatch();
		Flush();
		BeginBatch();
	}

	// Calculate the anchor point (Origin of the quad, 0,0 is bottom left)
	const auto anchor = glm::vec2(inPosition.x + (size.x * anchorPoint.x), inPosition.y + (size.y * anchorPoint.y));

	// Calculate the positions of the quad (anticlockwise)
	std::array<glm::vec3, 4> positions = { glm::vec3(anchor.x - (size.x / 2), anchor.y - (size.y / 2), 0.0f),
		                                   glm::vec3(anchor.x + (size.x / 2), anchor.y - (size.y / 2), 0.0f),
		                                   glm::vec3(anchor.x + (size.x / 2), anchor.y + (size.y / 2), 0.0f),
		                                   glm::vec3(anchor.x - (size.x / 2), anchor.y + (size.y / 2), 0.0f) };

	// Assuming rotation is in degrees
	const float rotationInRadians = glm::radians(rotation);

	// Use GLM's rotate function for vector rotation
	const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationInRadians, glm::vec3(0.0f, 0.0f, 1.0f));

	int i = 0;
	for (glm::vec3 position : positions)
	{
		// Apply rotation to each position
		position = glm::vec3(rotationMatrix * glm::vec4(position, 1.0f));

		// Adjust position based on anchor point
		position.x += anchor.x;
		position.y += anchor.y;

		// Add the quad to the batch
		data.quadBufferPtr->position  = position;
		data.quadBufferPtr->color     = color;
		data.quadBufferPtr->texCoords = basicUVS[i];
		data.quadBufferPtr->texIndex  = (float)texId;
		data.quadBufferPtr++;
		i++;
	}

	data.indexCount += 6;
}

void Renderer2D::DrawText(std::string text, glm::vec2 position, glm::vec4 color, float scale, std::string font)
{
	glUseProgram(m_TextShader->GetID());

	ShaderOpenGL::setVec3(*m_TextShader, "textColor", color);
	ShaderOpenGL::setMat4(*m_TextShader, "projection", glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f));

	glBindVertexArray(m_TextVAO);
	float x = position.x;
	float y = position.y;

	// Iterate through all characters
	for (auto c : text)
	{
		Character ch = CharacterManager::Instance().GetCharacter(c);

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		// Update VBO for each character
		float vertices[6][4] = { { xpos, ypos + h, 0.0, 0.0 }, { xpos, ypos, 0.0, 1.0 },     { xpos + w, ypos, 1.0, 1.0 },

			                     { xpos, ypos + h, 0.0, 0.0 }, { xpos + w, ypos, 1.0, 1.0 }, { xpos + w, ypos + h, 1.0, 0.0 } };

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, ch.texture);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer2D::Init(node::Camera* camera, ShaderOpenGL* basicShader, ShaderOpenGL* textShader)
{
	m_Camera = camera;

	m_BasicShader = basicShader;
	m_TextShader  = textShader;

	ShaderOpenGL::Use(*m_BasicShader);

	auto loc = glGetUniformLocation(m_BasicShader->GetID(), "Textures");
	int samplers[maxTextures];
	for (int i = 0; i < maxTextures; i++)
		samplers[i] = i;

	glUniform1iv(loc, maxTextures, samplers);

	data.quadBuffer = new Vertex[maxVertexCount];
	BeginBatch();

	glCreateVertexArrays(1, &data.quadVA);
	glBindVertexArray(data.quadVA);

	glCreateBuffers(1, &data.quadVB);
	glBindBuffer(GL_ARRAY_BUFFER, data.quadVB);
	glBufferData(GL_ARRAY_BUFFER, maxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(data.quadVA, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexArrayAttrib(data.quadVA, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexArrayAttrib(data.quadVA, 2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

	glEnableVertexArrayAttrib(data.quadVA, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));

	uint32_t indices[maxIndexCount];
	uint32_t offset = 0;
	for (int i = 0; i < maxIndexCount; i += 6)
	{
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}

	glCreateBuffers(1, &data.quadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.quadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glCreateTextures(GL_TEXTURE_2D, 1, &data.whiteTexture);
	glBindTexture(GL_TEXTURE_2D, data.whiteTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	uint32_t color = 0xffffffff;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

	data.textureSlots[0] = data.whiteTexture;
	for (size_t i = 1; i < maxTextures; i++)
	{
		data.textureSlots[i] = 0;
	}

	// Setup text rendering
	// TEXT STUFF BELOW --------------
	// Font stuff
	FT_Library library;
	FT_Face face; /* handle to face object */

	int error = FT_Init_FreeType(&library);
	S_ASSERT(!error, "Something went wrong")

	error = FT_New_Face(library, "../Resources/Fonts/Manrope-Regular.ttf", 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
		S_ASSERT(false, "Font file could be opened and read, but it appears that its font format is unsupported");
	}
	else if (error)
	{
		S_ASSERT(false, "The font file could not be opened or read, or maybe it is broken");
	}

	// Set font size
	error = FT_Set_Pixel_Sizes(face, 0, 32);
	S_ASSERT(!error, "Something went wrong")

	auto& charManager = CharacterManager::Instance();

	// load all glyphs
	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			S_ASSERT(false, "ERROR::FREETYTPE: Failed to load Glyph");
			continue;
		}
		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// generate texture
		unsigned int glyphTexture;
		glGenTextures(1, &glyphTexture);
		glBindTexture(GL_TEXTURE_2D, glyphTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for later use
		Character character = { // texture
			                    glyphTexture,
			                    // size
			                    glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			                    // bearing
			                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			                    // advance
			                    (unsigned int)face->glyph->advance.x
		};

		charManager.AddCharacter(c, character);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	// Text buffer stuff
	glGenVertexArrays(1, &m_TextVAO);
	glGenBuffers(1, &m_TextVBO);
	glBindVertexArray(m_TextVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);

	// Create a the buffer data for 2 positions and 2 texture coordinates
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

void Renderer2D::ShutDown()
{
	glDeleteVertexArrays(1, &data.quadVA);
	glDeleteBuffers(1, &data.quadVB);
	glDeleteBuffers(1, &data.quadIB);

	glDeleteTextures(1, &data.whiteTexture);

	delete[] data.quadBuffer;
}

void Renderer2D::BeginBatch()
{
	data.quadBufferPtr = data.quadBuffer;
}

void Renderer2D::EndBatch()
{
	GLsizeiptr size = (uint8_t*)data.quadBufferPtr - (uint8_t*)data.quadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, data.quadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.quadBuffer);
}

void Renderer2D::Flush()
{
	for (uint32_t i = 0; i < data.textureSlotIndex; i++)
		glBindTextureUnit(i, data.textureSlots[i]);

	glBindVertexArray(data.quadVA);
	glDrawElements(GL_TRIANGLES, data.indexCount, GL_UNSIGNED_INT, nullptr);

	data.indexCount       = 0;
	data.textureSlotIndex = 1;
}