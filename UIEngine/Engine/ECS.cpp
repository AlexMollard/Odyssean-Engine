#include "pch.h"

#include "ECS.h"

ECS* ECS::s_Instance = nullptr;
flecs::entity ECS::CreateEntity()
{
	return m_World.entity();
}

flecs::entity ECS::CreateQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	flecs::entity quad = Instance()->m_World.entity();
	quad.set([&](components::Quad& quad, components::Transform& transform) {
		transform.SetPosition(position);
		quad.SetSize(size);
		quad.SetColor(color);
	});
	return quad;
}

flecs::entity ECS::CreateText(const std::string& inText, const glm::vec3& position, const glm::vec4& color)
{
	flecs::entity textEntity = Instance()->m_World.entity();
	textEntity.set([&](components::Text& text, components::Transform& transform) {
		transform.SetPosition(position);
		text.SetColor(color);
		text.SetText(inText);
	});
	return textEntity;
}

void ECS::Init(const Renderer2D* renderer)
{
	// Create the components (Not needed I think)
	m_World.component<components::Text>();
	m_World.component<components::Quad>();

	// apply velocity to position
	m_World.system<components::Velocity, components::Transform>("ApplyVelocity")
		.kind(flecs::OnUpdate)
		.each([&](flecs::entity e, components::Velocity& velocity, components::Transform& transform) {
			transform.SetPosition(transform.GetPosition() + (velocity.GetVelocity() * delta_time()));
		});

	// Render Quad
	m_World.system<components::Quad, components::Transform>("RenderSprite")
		.kind(flecs::OnStore)
		.each([&](flecs::entity e, components::Quad& quad, components::Transform& transform) {
			renderer->DrawQuad(transform.GetPosition(), quad.GetSize() * glm::vec2(transform.GetScale()), quad.GetColor(),
				quad.GetAnchorPoint(), quad.GetTextureID());
		});

	// Render Text
	m_World.system<components::Text, components::Transform>("RenderText")
		.kind(flecs::OnStore)
		.each([&](flecs::entity e, components::Text& text, components::Transform& transform) {
			renderer->DrawText(text.GetText(), transform.GetPosition(), text.GetColor(), text.GetScale(), "");
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

ecs_ftime_t ECS::delta_time() const
{
	const ecs_world_info_t* stats = ecs_get_world_info(m_World);
	return stats->delta_time;
}