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
			ECS::CreateQuad(glm::vec3(x * 100, y * 100, 0), glm::vec2(80, 80), glm::vec4(0.2, 0.2, 0.8, 1));
		}
	}
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime, const Services* services) {}

void TestingScene::Draw(Window& window, const Services* services) {}
