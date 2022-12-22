#include "pch.h"

#include "TestingScene.h"

#include "ECS.h"

const float width  = 1920.0f;
const float height = 1080.0f;

void TestingScene::Enter()
{
	// testing text
	ECS::CreateText("Hello World!", glm::vec3(100, 950, 0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ECS::CreateText("Good Bye World!", glm::vec3(100, 1000, 0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			glm::vec4 colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			// pastel rainbow colours
			colour.r = (x / 10.0f) * 0.5f + 0.5f;
			colour.g = (y / 10.0f) * 0.5f + 0.5f;
			colour.b = 0.5f;

			ECS::CreateQuad(glm::vec3(x * 82 + 100, y * 82 + 100, 0), glm::vec2(80, 80), colour);
		}
	}
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime, const Services* services) {}

void TestingScene::Draw(Window& window, const Services* services) {}
