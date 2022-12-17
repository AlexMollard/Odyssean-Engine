#include "pch.h"
#include "Texture.h"

// Include sail for texture loading
//#include <sail/sail.h>

#include <sail/sail-c++/sail-c++.h>

using namespace components;

Texture::Texture(std::string dir)
{
	load(dir);
}

Texture::Texture(unsigned int* id)
{
	m_TextureID = *id;
}

Texture::~Texture() {}

void Texture::load(std::string dir) 
{
	//sail::image image(dir);
}
