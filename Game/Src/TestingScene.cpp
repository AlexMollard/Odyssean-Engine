#include "TestingScene.h"

#include "ECS.h"

const float width  = 1920.0f;
const float height = 1080.0f;

void TestingScene::Enter()
{
	ECS::CreateQuad(glm::vec3(100.0f), glm::vec2(100.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime, const Services* services) {}

void TestingScene::Draw(Window& window, const Services* services) {}
