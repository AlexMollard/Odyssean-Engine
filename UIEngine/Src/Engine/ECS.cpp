#include "pch.h"

#include "ECS.h"

#include "imgui.h"

static int           s_IDIncrementor = 0;
static Node          s_RootNode;
static flecs::entity s_RootEntity;

ECS*          ECS::s_Instance = nullptr;
flecs::entity ECS::CreateEntity()
{
	return m_World.entity();
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

	flecs::entity quadEntity = Instance()->m_World.entity();
	quadEntity.set([&](node::Quad& quad, node::Transform& transform, node::Tag& tag) {
		transform.SetPosition(position);
		quad.SetSize(size);
		quad.SetColor(color);
		tag.SetName(newName.c_str());
	});

	quadEntity.set_name(newName.c_str());

	return *quadEntity.get_ref<flecs::entity>().get();
}

flecs::entity& ECS::CreateText(const std::string& inText, const glm::vec3& position, const glm::vec4& color)
{
	std::ostringstream out;
	out << "Text: " << s_IDIncrementor++;

	flecs::entity textEntity = Instance()->m_World.entity();
	textEntity.set([&](node::Text& text, node::Transform& transform, node::Tag& tag) {
		transform.SetPosition(position);
		text.SetColor(color);
		text.SetText(inText);
		tag.SetName(out.str().c_str());
	});
	textEntity.set_name(out.str().c_str());

	return *textEntity.get_ref<flecs::entity>().get();
}

void ECS::Init()
{
	m_World.import <flecs::units>();
	m_World.import <flecs::monitor>(); // Collect statistics periodically
	m_World.set<flecs::Rest>({});
	//m_World.app().enable_rest().enable_monitor().run();

	// Add a observer to the world that will be triggered when a new entity is created
	//m_World.observer<node::Tag>().event(flecs::OnAdd).each([&](flecs::iter& it, size_t i, node::Tag tag) {
	//	// Get the entity
	//	flecs::entity e = it.entity(i);

	//	// If the root entity is not set, set it
	//	if (!m_RootEntity) { m_RootEntity = e; }
	//});
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