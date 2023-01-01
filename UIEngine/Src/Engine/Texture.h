#pragma once
#include <string>

class Texture
{
public:
	Texture(std::string dir);
	Texture(unsigned int* id);
	Texture() = default;
	~Texture();

	void         Bind();
	unsigned int GetID();
	void         SetID(unsigned int newID);

	unsigned int GetWidth();
	unsigned int GetHeight();

	void SetWidth(unsigned int newWidth);
	void SetHeight(unsigned int newHeight);

	unsigned int m_TextureID  = 0;
	unsigned int m_Width      = 0;
	unsigned int m_Height     = 0;
	int          m_NrChannels = 4;
};
