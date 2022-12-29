#pragma once
#include "gl/glew.h"
#include <GLFW/glfw3.h>
#include <string>

class Texture
{
public:
	Texture(std::string dir);
	Texture(unsigned int* id);
	Texture() = default;
	~Texture();

	void Bind() { glBindTexture(GL_TEXTURE_2D, m_TextureID); };
	unsigned int GetID() { return m_TextureID; };
	void SetID(unsigned int newID) { m_TextureID = newID; };

	unsigned int GetWidth() { return m_Width; };
	unsigned int GetHeight() { return m_Height; };

	void SetWidth(unsigned int newWidth) { m_Width = newWidth; };
	void SetHeight(unsigned int newHeight) { m_Height = newHeight; };

	unsigned int m_TextureID = 0;
	unsigned int m_Width     = 0;
	unsigned int m_Height    = 0;
	int m_NrChannels         = 4;
};
