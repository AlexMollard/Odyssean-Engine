#include "TestingScene.h"

#include "ECS.h"
#include "glm/gtc/noise.hpp"

void TestingScene::Enter()
{
	int width        = 800;
	int height       = 600;
	float quadWidth  = 25.0f;
	float quadHeight = 25.0f;

	// If the quads are width * width then how many quads can fit in the screen
	float quadsPerRow    = width / quadWidth;
	float quadsPerColumn = height / quadHeight;

	// Create a quad for each row and column
	for (int i = 0; i < quadsPerRow; i++)
	{
		for (int j = 0; j < quadsPerColumn; j++)
		{
			float noise = glm::perlin(glm::vec2(i, j) * 0.1f);
			ECS::CreateQuad(glm::vec3((i * quadWidth) + 100, (j * quadHeight) + 100, 0.0f), glm::vec2(quadWidth, quadHeight),
				glm::vec4(0.0f, noise, noise, 1.0f));
		}
	}
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime) {}

void TestingScene::Draw() {}
