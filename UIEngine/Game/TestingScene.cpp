#include "pch.h"

#include "TestingScene.h"

#include "ECS.h"

const float width  = 1920.0f;
const float height = 1080.0f;

void TestingScene::Enter()
{
	for (int x = 0; x < 100; x++)
	{
		for (int y = 0; y < 100; y++)
		{
			glm::vec4 colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			// pastel rainbow colours
			colour.r = (x / 100.0f) * 0.5f + 0.5f;
			colour.g = (y / 100.0f) * 0.5f + 0.5f;
			colour.b = 0.5f;

			ECS::CreateQuad(glm::vec3(x * 9 + 100, y * 9 + 100, 0), glm::vec2(8, 8), colour);
		}
	}
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime, const Services* services) {}

void TestingScene::Draw(Window& window, const Services* services) {}
