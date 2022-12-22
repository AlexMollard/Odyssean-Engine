#pragma once
// Helper functions that can be passed around to other classes
#include "Audio.h"
#include "Renderer2D.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"

class Window;
class Services
{
public:
	Services(const Window* window) : window(window) {}

	// Get and set the window
	void SetWindow(const Window* inWindow) { window = inWindow; }
	const Window* GetWindow() const { return window; }

	// Get and set the renderer
	void SetRenderer(const Renderer2D* inRenderer) { renderer = inRenderer; }
	const Renderer2D* GetRenderer() const { return renderer; }

	// Get and set the Shader manager
	void SetShaderManager(ResourceManager<Shader>* inShaderManager) { shaderManager = inShaderManager; }
	ResourceManager<Shader>* GetShaderManager() const { return shaderManager; }

	// Get and set texture manager
	void SetTextureManager(ResourceManager<Texture>* inTextureManager) { textureManager = inTextureManager; }
	ResourceManager<Texture>* GetTextureManager() const { return textureManager; }

	// Get and set audio manager
	void SetAudioManager(Audio* inAudioManager) { audioManager = inAudioManager; }
	Audio* GetAudioManager() const { return audioManager; }

private:
	const Window* window       = nullptr;
	const Renderer2D* renderer = nullptr;
	Audio* audioManager        = nullptr;

	ResourceManager<Shader>* shaderManager           = nullptr;
	ResourceManager<Texture>* textureManager         = nullptr;
	ResourceManager<class Mesh>* meshManager         = nullptr;
	ResourceManager<class Material>* materialManager = nullptr;
	ResourceManager<class Model>* modelManager       = nullptr;
};
