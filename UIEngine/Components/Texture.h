#pragma once
#include "gl/glew.h"
#include <GLFW/glfw3.h>
#include <string>

namespace components
{
class Texture
{
public:
	Texture(std::string dir);
	Texture(unsigned int* id);
	Texture(){};
	~Texture();

	void load(std::string dir);
	void Bind() { glBindTexture(GL_TEXTURE_2D, m_TextureID); };
	unsigned int GetID() { return m_TextureID; };
	void SetID(unsigned int newID) { m_TextureID = newID; };

	int GetWidth() { return m_Width; };
	int GetHeight() { return m_Height; };

private:
	unsigned int m_TextureID = 0;
	int m_Width              = 0;
	int m_Height             = 0;
	//int m_NrChannels         = 4;
};
} // namespace components