#include "pch.h"

#include "Texture.h"

#include <sail/sail-c++/sail-c++.h>

components::Texture::Texture(std::string dir)
{
	// Create and bind texture ID
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	// Set Wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	const unsigned char* imageData = loadImage(dir);
	S_WARN(imageData, "Failed to load texture");

	if (imageData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, m_NrChannels != 4 ? GL_RGB : GL_RGBA, m_Width, m_Height, 0, m_NrChannels != 4 ? GL_RGB : GL_RGBA,
			GL_UNSIGNED_BYTE, imageData);
	}
}

components::Texture::Texture(unsigned int* id) : m_TextureID(*id) {}

components::Texture::~Texture() = default;

unsigned char* components::Texture::loadImage(std::string& dir)
{
	sail::image image(dir);

	// Now that we have loaded the image, we can get the data from it
	// and convert it to a format that OpenGL can use
	SetWidth(100);
	SetHeight(image.height());

	// Get the data from the image
	auto* data = static_cast<unsigned char*>(image.pixels());

	return data;
}
