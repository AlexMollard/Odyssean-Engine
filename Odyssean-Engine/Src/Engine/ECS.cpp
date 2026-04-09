#include "pch.h"
#include "BS_thread_pool.hpp"

#include "ECS.h"

#include "imgui.h"

namespace
{
int s_IDIncrementor = 0;
}

ECS* ECS::s_Instance = nullptr;

flecs::entity ECS::CreateEntity()
{
	flecs::entity entity = m_World.entity();
	entity.child_of(GetRootEntity());
	return entity;
}

flecs::entity* ECS::CreateQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const char* name)
{
	s_IDIncrementor++;
	std::string entityName   = name ? name : std::format("Quad: {}", s_IDIncrementor);
	flecs::entity quadEntity = Instance()->CreateEntity();
	node::Transform transform;
	transform.SetPosition(position);
	quadEntity.set<node::Transform>(transform);

	node::Quad quad;
	quad.SetSize(size);
	quad.SetColor(color);
	quadEntity.set<node::Quad>(quad);

	node::Tag tag;
	tag.SetName(entityName.c_str());
	quadEntity.set<node::Tag>(tag);

	quadEntity.set_name(entityName.c_str());
	return quadEntity.get_ref<flecs::entity>().get();
}

flecs::entity* ECS::CreateText(const std::string& inText, const glm::vec3& position, const glm::vec4& color)
{
	s_IDIncrementor++;
	std::string entityName   = std::format("Text: {}", s_IDIncrementor);
	flecs::entity textEntity = Instance()->CreateEntity();
	node::Transform transform;
	transform.SetPosition(position);
	textEntity.set<node::Transform>(transform);

	node::Text text;
	text.SetColor(color);
	text.SetText(inText);
	textEntity.set<node::Text>(text);

	node::Tag tag;
	tag.SetName(entityName.c_str());
	textEntity.set<node::Tag>(tag);

	textEntity.set_name(entityName.c_str());
	return textEntity.get_ref<flecs::entity>().get();
}

void ECS::GetNodeInspectorFunction(const flecs::entity& entity)
{
	if (entity.has<node::Transform>())
	{
		entity.get_ref<node::Transform>()->Inspector();
	}
	if (entity.has<node::Quad>())
	{
		entity.get_ref<node::Quad>()->Inspector();
	}
	if (entity.has<node::Text>())
	{
		entity.get_ref<node::Text>()->Inspector();
	}
}

void ECS::Init()
{
	m_World.import <flecs::units>();
	m_World.set<flecs::Rest>({});
	m_RootEntity = m_World.entity();
	m_RootEntity.add<node::Transform>();
	m_RootEntity.set_name("RootNode");
}

void ECS::Update(const BS::thread_pool<>& pool)
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