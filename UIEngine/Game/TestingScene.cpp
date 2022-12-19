#include "pch.h"

#include "TestingScene.h"

#include "ECS.h"

const float width  = 1920.0f;
const float height = 1080.0f;

void TestingScene::Enter()
{
	for (int i = 0; i < 10; i++)
	{
		ECS::CreateQuad(glm::vec3(100, 100 * (i + 1), 0), glm::vec2(80, 80), glm::vec4(0.8, 0.2, 0.2, 1));
		ECS::CreateText("TESTING", glm::vec3(100, 100 * (i + 1), 0), glm::vec4(0.2, 0.8, 0.2, 1));
	}
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime, const Services* services) {}

void TestingScene::Draw(Window& window, const Services* services) {}
