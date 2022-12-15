#include "pch.h"
#include "Renderer2d.h"
#include "ECS.h"

ECS* ECS::s_Instance = nullptr;
flecs::entity ECS::CreateEntity()
{
	return m_World.entity();
}

void ECS::Init()
{
	// Create the components (Not needed I think)
	m_World.component<components::Text>(); // Change this class name
	m_World.component<components::Quad>();

	// Render Quad
	m_World.system<components::Quad>("RenderSprite").kind(flecs::OnStore).each([](flecs::entity e, components::Quad& quad) {
		//Renderer::RenderQuad(quad);
	});

	// Render Text
	m_World.system<components::Text>("RenderText").kind(flecs::OnStore).each([](flecs::entity e, components::Text& text) {
		//Renderer2d::RenderText(text);
	});
}

void ECS::Update()
{
	// Update the ECS
	m_World.progress();
}

void ECS::Destroy()
{
	// Delete the singleton instance
	delete s_Instance;
	s_Instance = nullptr;
}
