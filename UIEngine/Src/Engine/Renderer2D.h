#pragma once
#include "Components/Camera.h"
#include "Components/Quad.h"
#include <freetype/freetype.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>


class Shader;
class Texture;

class Renderer2D
{
public:
	Renderer2D() = default;
	~Renderer2D();

	void Init(components::Camera* camera, Shader* basicShader, Shader* textShader);
	void ShutDown();

	void Draw();

	static void DrawQuad(glm::vec2 position, glm::vec2 size, glm::vec4 color, glm::vec2 anchorPoint, const unsigned int texId);

	static void DrawText(std::string text, glm::vec2 position, glm::vec4 color, float scale, std::string font);

	static void BeginBatch();
	static void EndBatch();
	static void Flush();

private:
	glm::vec3 currentColor  = glm::vec3(-404);
	Shader* currentShader   = nullptr;
	Texture* currentTexture = nullptr;

	std::vector<Texture*> texturePool;
	std::vector<Shader*> shaderPool;

	Shader* m_BasicShader;
	components::Camera* m_Camera;
};