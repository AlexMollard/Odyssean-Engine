#include "pch.h"

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
	quadEntity.set(
	    [&position, &size, &color, &entityName](node::Quad& quad, node::Transform& transform, node::Tag& tag)
	    {
		    transform.SetPosition(position);
		    quad.SetSize(size);
		    quad.SetColor(color);
		    tag.SetName(entityName.c_str());
	    });
	quadEntity.set_name(entityName.c_str());
	return quadEntity.get_ref<flecs::entity>().get();
}

flecs::entity* ECS::CreateText(const std::string& inText, const glm::vec3& position, const glm::vec4& color)
{
	s_IDIncrementor++;
	std::string entityName   = std::format("Text: {}", s_IDIncrementor);
	flecs::entity textEntity = Instance()->CreateEntity();
	textEntity.set(
	    [&position, &color, &inText, &entityName](node::Text& text, node::Transform& transform, node::Tag& tag)
	    {
		    transform.SetPosition(position);
		    text.SetColor(color);
		    text.SetText(inText);
		    tag.SetName(entityName.c_str());
	    });
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
	m_World.import <flecs::monitor>();
	m_World.set<flecs::Rest>({});
	m_RootEntity = m_World.entity();
	m_RootEntity.add<node::Transform>();
	m_RootEntity.set_name("RootNode");
}

void ECS::Update(const BS::thread_pool& pool)
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