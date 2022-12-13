#pragma once
#include "Scene.h"
#include <freetype/freetype.h>
#include <map>
#include <memory>

class Shader;
class Window;
class Services;
struct Character
{
	unsigned int texture;
	glm::vec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

// Static class to hold all the characters
class CharacterManager
{
public:
	static CharacterManager& Instance()
	{
		static CharacterManager instance;
		return instance;
	}

	CharacterManager(CharacterManager const&) = delete;
	void operator=(CharacterManager const&)   = delete;

	void AddCharacter(char c, Character character) { Characters.insert(std::pair<char, Character>(c, character)); }

	Character GetCharacter(char c) { return Characters[c]; }

private:
	CharacterManager() = default;
	std::map<char, Character> Characters{};
};

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
	unsigned int VAO     = 0;
	unsigned int VBO     = 0;
	unsigned int EBO     = 0;
	unsigned int texture = 0;

	unsigned int textVAO;
	unsigned int textVBO;

	// Font stuff
	FT_Library library;
	FT_Face face; /* handle to face object */

	glm::mat4 model      = glm::mat4(1.0f);
	glm::mat4 view       = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	glm::vec3 direction = glm::vec3(0.2f, 0.2f, 0.0f);
};
