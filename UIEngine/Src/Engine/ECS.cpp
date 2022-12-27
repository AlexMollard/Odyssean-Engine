#include "pch.h"

#include "ECS.h"

#include "imgui.h"
#include <Tracy.hpp>

static int s_IDIncrementor = 0;

ECS* ECS::s_Instance = nullptr;
flecs::entity ECS::CreateEntity()
{
	ZoneScoped;
	return m_World.entity();
}

flecs::entity& ECS::CreateQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	ZoneScoped;
	std::ostringstream out;
	out << "Quad: " << s_IDIncrementor++;

	flecs::entity quadEntity = Instance()->m_World.entity();
	quadEntity.set([&](components::Quad& quad, components::Transform& transform, components::Tag& tag) {
		transform.SetPosition(position);
		quad.SetSize(size);
		quad.SetColor(color);
		tag.SetName(out.str());
	});

	quadEntity.set_name(out.str().c_str());

	return *quadEntity.get_ref<flecs::entity>().get();
}

flecs::entity& ECS::CreateText(const std::string& inText, const glm::vec3& position, const glm::vec4& color)
{
	ZoneScoped;
	std::ostringstream out;
	out << "Text: " << s_IDIncrementor++;

	flecs::entity textEntity = Instance()->m_World.entity();
	textEntity.set([&](components::Text& text, components::Transform& transform, components::Tag& tag) {
		transform.SetPosition(position);
		text.SetColor(color);
		text.SetText(inText);
		tag.SetName(out.str());
	});
	textEntity.set_name(out.str().c_str());

	return *textEntity.get_ref<flecs::entity>().get();
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
	ZoneScopedN("ECS Update");
	// Update the ECS
	m_World.progress();

	// Fps counter
	ImGui::Begin("Stats");
	{
		ZoneScopedN("Stats");
		const ecs_world_info_t* stats = ecs_get_world_info(m_World);

		m_FPS = 1.0f / stats->delta_time;
		ImGui::Text("FPS: %f", m_FPS);
		ImGui::Text("High FPS: %f", m_FPSHigh);
		ImGui::Text("Low FPS: %f", m_FPSLow);

		// add fps to a vector
		m_FpsHistory.push_back(m_FPS);
		// if the vector is bigger than 100, remove the first element
		if (m_FpsHistory.size() > 400)
		{
			m_FpsHistory.erase(m_FpsHistory.begin());
		}

		// Average fps
		float averageFps = 0.0f;
		for (auto& fps : m_FpsHistory)
		{
			averageFps += fps;
		}
		averageFps /= m_FpsHistory.size();
		ImGui::Text("Average FPS: %f", averageFps);

		// Store Average FPS for plot graph
		m_AverageFpsHistory.push_back(averageFps);
		// if the vector is bigger than 100, remove the first element
		if (m_AverageFpsHistory.size() > 400)
		{
			m_AverageFpsHistory.erase(m_AverageFpsHistory.begin());
		}

		// Draw the fps graph
		ImGui::PlotLines("FPS", m_AverageFpsHistory.data(), m_AverageFpsHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));

		m_FPSTimer += stats->delta_time;

		if (m_FPSTimer >= 1.0f)
		{
			m_FPSTimer = 0.0f;
			m_FPSHigh  = m_FPS;
			m_FPSLow   = m_FPS;
		}
		else
		{
			if (m_FPS > m_FPSHigh)
			{
				m_FPSHigh = m_FPS;
			}
			if (m_FPS < m_FPSLow)
			{
				m_FPSLow = m_FPS;
			}
		}
	}
	ImGui::End();
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