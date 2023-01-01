#include "pch.h"

#include "TestingScene.h"

#include "glm/gtc/noise.hpp"

#include "Engine/ECS.h"

void TestingScene::Enter()
{
	int   width      = 1920;
	int   height     = 1080;
	float quadWidth  = 25.0f;
	float quadHeight = 25.0f;
	//ECS::CreateQuad(glm::vec3(pos), glm::vec2(size), glm::vec4(color), const char* name);

	// So the quads anchor point is {0.5, 0.5} ^_^

	// Create a quad at each corner of the screen and a quad in the center
	// Colors are based on the position of the quad (top left is red, top right is green, bottom left is blue, bottom right is yellow, center is white)
	ECS::CreateQuad(glm::vec3(0, height, 0), glm::vec2(quadWidth, quadHeight), glm::vec4(1, 0, 0, 1), "TopLeftQuad");
	ECS::CreateQuad(glm::vec3(width, height, 0), glm::vec2(quadWidth, quadHeight), glm::vec4(0, 1, 0, 1), "TopRightQuad");
	ECS::CreateQuad(glm::vec3(0, 0, 0), glm::vec2(quadWidth, quadHeight), glm::vec4(0, 0, 1, 1), "BottomLeftQuad");
	ECS::CreateQuad(glm::vec3(width, 0, 0), glm::vec2(quadWidth, quadHeight), glm::vec4(1, 1, 0, 1), "BottomRightQuad");
	ECS::CreateQuad(glm::vec3(width / 2, height / 2, 0), glm::vec2(quadWidth, quadHeight), glm::vec4(1, 1, 1, 1), "CenterQuad");

	// Circle the center quad around the screen
	float x = 0;
	float y = 0;
	float total = 100;
	float radius = 400.0f;

	for (int i = 0; i < total; i++)
	{
		float angle = (i / total) * 360.0f;
		x = width / 2 + radius * cos(glm::radians(angle));
		y = height / 2 + radius * sin(glm::radians(angle));

		ECS::CreateQuad(glm::vec3(x, y, 0), glm::vec2(quadWidth, quadHeight), glm::vec4(1, 1, 1, 1), std::to_string(x / y).c_str());
	}


	m_Renderer   = _NEW   Renderer2D();
	m_litShader  = _NEW  ShaderOpenGL("LitShader", "../Resources/Shaders/lit.vert", "../Resources/Shaders/lit.frag");
	m_textShader = _NEW ShaderOpenGL("TextShader", "../Resources/Shaders/font.vert", "../Resources/Shaders/font.frag");
	m_Renderer->Init(nullptr, m_litShader, m_textShader);
}

void TestingScene::Exit()
{
	delete m_textShader;
	delete m_litShader;
	delete m_Renderer;
}

void TestingScene::Update(float deltaTime)
{
}

void TestingScene::Draw(const BaseRenderer& renderer)
{
	auto& world = ECS::GetWorldStatic();

	flecs::filter<node::Quad, node::Transform> f = world.filter<node::Quad, node::Transform>();
	f.each([this](node::Quad& q, node::Transform& t) { m_Renderer->DrawQuad(t.GetPosition(), q.GetSize() * t.GetScale(), q.GetColor(), q.GetAnchorPoint(), 0); });

	m_Renderer->Draw();
}