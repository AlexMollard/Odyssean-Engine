#pragma once
#include "Scene.h"
#include <memory>
class Shader;
class Window;
class Services;
class TestingScene : public Scene
{
public:
	explicit TestingScene(std::string_view inName) { name = inName; };

	void OnCreate() override{ /*To Be Impelmented*/ };
	void Enter() override;
	
	void Exit() override;
	void OnDestroy() override{ /*To Be Impelmented*/ };

	void Update(float deltaTime, const Services* services) override;
	void Draw(Window& window, const Services* services) override;

private:
	std::string name;
	// Anything needed for the scene goes here
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	glm::mat4 model      = glm::mat4(1.0f);
	glm::mat4 view       = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	glm::vec3 direction = glm::vec3(0.2f, 0.2f, 0.0f);
};
