#include "pch.h"

#include "ECS.h"

ECS* ECS::s_Instance = nullptr;
flecs::entity ECS::CreateEntity()
{
	return m_World.entity();
}

void ECS::Init(const Renderer2D* renderer)
{
	// Create the components (Not needed I think)
	m_World.component<components::Text>();
	m_World.component<components::Quad>();

	// Render Quad
	m_World.system<components::Quad>("RenderSprite").kind(flecs::OnStore).each([&](flecs::entity e, components::Quad& quad) {
		renderer->DrawQuad(quad.GetPosition(), quad.GetSize(), quad.GetColor(), quad.GetAnchorPoint(), quad.GetTextureID());
	});

	// Render Text
	m_World.system<components::Text>("RenderText").kind(flecs::OnStore).each([&](flecs::entity e, components::Text& text) {
		renderer->DrawText(text.GetText(), text.GetPosition(), text.GetColor(), text.GetScale(), "");
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
