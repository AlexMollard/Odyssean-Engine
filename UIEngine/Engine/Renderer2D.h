#pragma once
#include "Camera.h"
#include "Quad.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <vector>

class Shader;

class Renderer2D
{
public:
	Renderer2D(components::Camera* camera);
	~Renderer2D();

	static void Init();
	static void ShutDown();

	void AddObject(components::Quad* newObject);

	components::Texture* LoadTexture(std::string dir);

	void Draw();
	void DrawUI();

	Shader* GetBasicShader();

	static void setActiveRegion(components::Texture* texture, int regionIndex, int spriteWidth);

	static void DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color);
	static void DrawQuad(glm::vec2 position, glm::vec2 size, glm::vec4 color) { DrawQuad(glm::vec3(position, -0.9f), size, color); };
	static void DrawQuad(
		glm::vec3 position, glm::vec2 size, glm::vec4 color, components::Texture* texture, int frame = 0, int spriteWidth = 16);

	void RemoveQuad(components::Quad* object);
	int GetObjectIndex(components::Quad* object);

	static void BeginBatch();
	static void EndBatch();
	static void Flush();

private:
	glm::vec3 currentColor              = glm::vec3(-404);
	Shader* currentShader               = nullptr;
	components::Texture* currentTexture = nullptr;

	std::vector<components::Quad*> objectPool;
	std::vector<components::Texture*> texturePool;
	std::vector<Shader*> shaderPool;

	static Shader* basicShader;
	static components::Camera* camera;

	static std::vector<glm::vec2> UVs;
};