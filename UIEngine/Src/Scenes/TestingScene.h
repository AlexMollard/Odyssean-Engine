#pragma once
#include "glm/glm.hpp"
#include <string_view>

#include "Engine/OpenGLAPI/ShaderOpenGL.h"
#include "Engine/Renderer2D.h"
#include "Engine/Scene.h"

class TestingScene : public Scene
{
public:
	explicit TestingScene(std::string_view inName) : name(inName){};

	void OnCreate() override{ /*To Be Impelmented*/ };
	void Enter() override;

	void Exit() override;
	void OnDestroy() override{ /*To Be Impelmented*/ };

	void Update(float deltaTime) override;
	void Draw() override;

private:
	std::string name;

	glm::vec3 direction = glm::vec3(0.2f, 0.2f, 0.0f);

	std::shared_ptr<ShaderOpenGL> m_litShader  = nullptr;
	std::shared_ptr<ShaderOpenGL> m_textShader = nullptr;
	std::shared_ptr<Renderer2D> m_Renderer     = nullptr;
};