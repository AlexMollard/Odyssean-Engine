#include "pch.h"

#include "TestingScene.h"

#include "ECS.h"

const float width  = 1920.0f;
const float height = 1080.0f;

void TestingScene::Enter()
{
	flecs::entity quad = ECS::Instance()->CreateEntity();
	quad.set([](components::Quad& quad) {
		quad.SetPosition(glm::vec2(600.0f, 800.0f));
		quad.SetSize(glm::vec2(100.0f, 100.0f));
		quad.SetColor(glm::vec4(0.2f, 0.8f, 0.8f, 1.0f));
		quad.SetAnchorPoint(glm::vec2(0.5f, 0.5f));
		quad.SetTextureID(0);
	});

	flecs::entity textTest = ECS::Instance()->CreateEntity();
	textTest.set([](components::Text& text) {
		text.SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		text.SetPosition(glm::vec3(100.0f, 100.0f, 0.0f));
		text.SetText("Yeah we got text!");
	});
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime, const Services* services) {}

void TestingScene::Draw(Window& window, const Services* services) {}
