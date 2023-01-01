#pragma once
#include "glm/glm.hpp"
#include <string_view>

#include "Engine/Scene.h"

class VulkanScene : public Scene
{
public:
	explicit VulkanScene(std::string_view inName)
	{
		name = inName;
	};

	void OnCreate() override{ /*To Be Impelmented*/ };
	void Enter() override;

	void Exit() override;
	void OnDestroy() override{ /*To Be Impelmented*/ };

	void Update(float deltaTime) override;
	void Draw(const BaseRenderer& renderer) override;

private:
	std::string name;

	glm::vec3 direction = glm::vec3(0.2f, 0.2f, 0.0f);
};