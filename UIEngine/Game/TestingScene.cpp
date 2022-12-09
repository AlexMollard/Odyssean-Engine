#include "pch.h"

#include "TestingScene.h"

#include "ResourceManager.h"
#include "Services.h"
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"

const float width  = 1920.0f;
const float height = 1080.0f;

void TestingScene::Enter()
{
	// Create a triangle VBO
	float vertices[] = {
		// Pos              // Colour
		0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.0f,    // top right
		0.5f, -0.5f, 0.0f, 0.0f, 0.75f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.35f, 1.0f, // bottom left
		-0.5f, 0.5f, 0.0f, 1.0f, 0.15f, 1.0f,  // top left
	};
	unsigned int indices[] = {
		// note that we start from 0!
		0, 3, 1, // first triangle
		1, 3, 2  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 5. now set the colours
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Setup the projection matrix to be orthographic
	projection = glm::ortho(-(width / 2.0f), width / 2.0f, height / 2.0f, -(height / 2.0f), -1000.0f, 1000.0f);

	// Set the model matrix
	model = glm::mat4(1.0f);

	//Move square to top left
	model = glm::translate(model, glm::vec3(-width / 2.0f + 60.0f, -height / 2.0f + 60.0f, 0.0f));

	// Move the quad to the bottom left corner
	model = glm::scale(model, glm::vec3(100.0f, 100.0f, 1.0f));
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime, const Services* services)
{
	S_INFO(name)
	float dt = deltaTime;

	// Make the quad bounce off the edges
	if (model[3][0] > width / 2.0f - 50.0f)
	{
		direction += glm::vec3(-dt, 0.0f, 0.0f);
	}
	else if (model[3][0] < -width / 2.0f + 50.0f)
	{
		direction += glm::vec3(dt, 0.0f, 0.0f);
	}

	if (model[3][1] > height / 2.0f - 50.0f)
	{
		direction += glm::vec3(0.0f, -dt, 0.0f);
	}
	else if (model[3][1] < -height / 2.0f + 50.0f)
	{
		direction += glm::vec3(0.0f, dt, 0.0f);
	}

	// clamp the direction
	direction = glm::clamp(direction, glm::vec3(-0.02f, -0.02f, 0.0f), glm::vec3(0.02f, 0.02f, 0.0f));

	// Move
	model = glm::translate(model, direction);
}

void TestingScene::Draw(Window& window, const Services* services)
{
	glBindVertexArray(VAO);

	const Shader* mainShader = services->GetShaderManager()->Load("Shaders/lit.vert").get();
	glUseProgram(mainShader->GetID());

	// Set MVP
	mainShader->setMat4("model", model);
	mainShader->setMat4("view", view);
	mainShader->setMat4("projection", projection);

	// Draw the Quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}
