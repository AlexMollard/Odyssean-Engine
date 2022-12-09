#pragma once
// Helper functions that can be passed around to other classes
#include "ResourceManager.h"
#include "Shader.h"

class Window;
class Texture;
class Services
{
public:
	Services(const Window* window) : window(window) {}

	// Get and set the window
	void SetWindow(Window const* inWindow) { window = inWindow; }
	const Window* GetWindow() const { return window; }

	// Get and set the Shader manager
	void SetShaderManager(ResourceManager<Shader>* inShaderManager) { shaderManager = inShaderManager; }
	ResourceManager<Shader>* GetShaderManager() const { return shaderManager; }

	// Get and set texture manager
	void SetTextureManager(ResourceManager<Texture>* inTextureManager) { textureManager = inTextureManager; }
	ResourceManager<Texture>* GetTextureManager() const { return textureManager; }

private:
	const Window* window;

	ResourceManager<Shader>* shaderManager           = nullptr;
	ResourceManager<Texture>* textureManager   = nullptr;
	ResourceManager<class Mesh>* meshManager         = nullptr;
	ResourceManager<class Material>* materialManager = nullptr;
	ResourceManager<class Model>* modelManager       = nullptr;
};
