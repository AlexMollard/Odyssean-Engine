#pragma once
#include "Nodes/Camera.h"
#include "Nodes/Quad.h"
#include <freetype/freetype.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class ShaderOpenGL;
class Texture;

class Renderer2D
{
public:
	Renderer2D() = default;
	~Renderer2D();

	void Init(node::Camera* camera, ShaderOpenGL* basicShader, ShaderOpenGL* textShader);
	void ShutDown();

	void Draw();

	static void DrawQuad(glm::vec2 position, glm::vec2 size, glm::vec4 color, glm::vec2 anchorPoint, float rotation, const unsigned int texId);

	static void DrawText(std::string text, glm::vec2 position, glm::vec4 color, float scale, std::string font);

	static void BeginBatch();
	static void EndBatch();
	static void Flush();

private:
	glm::vec3 currentColor      = glm::vec3(-404);
	ShaderOpenGL* currentShader = nullptr;
	Texture* currentTexture     = nullptr;

	std::vector<Texture*> texturePool;
	std::vector<ShaderOpenGL*> shaderPool;

	ShaderOpenGL* m_BasicShader;
	node::Camera* m_Camera;
};