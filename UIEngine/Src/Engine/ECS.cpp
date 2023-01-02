#include "pch.h"

#include "ECS.h"

#include "imgui.h"

static int s_IDIncrementor = 0;

ECS* ECS::s_Instance = nullptr;

flecs::entity ECS::CreateEntity()
{
	flecs::entity entity = m_World.entity();
	entity.child_of(GetRootEntity());
	// Add the entity to the root node
	return entity;
}

flecs::entity& ECS::CreateQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const char* name)
{
	std::string newName = name;

	if (name == nullptr)
	{
		std::ostringstream out;
		out << "Quad: " << s_IDIncrementor++;
		newName = out.str();
	}

	flecs::entity quadEntity = Instance()->CreateEntity();
	quadEntity.set([&](node::Quad& quad, node::Transform& transform, node::Tag& tag) {
		transform.SetPosition(position);
		quad.SetSize(size);
		quad.SetColor(color);
		tag.SetName(newName.c_str());
	});

	quadEntity.set_name(newName.c_str());

	// flecs stores the entity as a int64_t, so we need to cast it to a pointer
	return quadEntity;
}

flecs::entity* ECS::CreateText(const std::string& inText, const glm::vec3& position, const glm::vec4& color)
{
	std::ostringstream out;
	out << "Text: " << s_IDIncrementor++;

	flecs::entity textEntity = Instance()->CreateEntity();
	textEntity.set([&](node::Text& text, node::Transform& transform, node::Tag& tag) {
		transform.SetPosition(position);
		text.SetColor(color);
		text.SetText(inText);
		tag.SetName(out.str().c_str());
	});
	textEntity.set_name(out.str().c_str());

	return textEntity.get_ref<flecs::entity>().get();
}

void ECS::GetNodeInspectorFunction(flecs::entity& entity)
{
	// Transform
	if (entity.has<node::Transform>()) { entity.get_ref<node::Transform>()->Inspector(); }

	// Quad
	if (entity.has<node::Quad>()) { entity.get_ref<node::Quad>()->Inspector(); }

	// Text
	if (entity.has<node::Text>()) { entity.get_ref<node::Text>()->Inspector(); }
}

void ECS::Init()
{
	m_World.import <flecs::units>();
	m_World.import <flecs::monitor>(); // Collect statistics periodically
	m_World.set<flecs::Rest>({});

	// Add the root node/Entity
	m_RootEntity = m_World.entity();
	m_RootEntity.add<node::Transform>();
	m_RootEntity.set_name("RootNode");
}

void ECS::Update(BS::thread_pool& pool)
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