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
		// positions          // colors           // texture coords
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 5. now set the colours
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 6. now set the texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Setup the projection matrix to be orthographic
	projection = glm::ortho(-(width / 2.0f), width / 2.0f, height / 2.0f, -(height / 2.0f), -1000.0f, 1000.0f);

	// Set the model matrix
	model = glm::mat4(1.0f);

	//Move square to top left
	model = glm::translate(model, glm::vec3(-width / 2.0f + 60.0f, -height / 2.0f + 60.0f, 0.0f));

	// Move the quad to the bottom left corner
	model = glm::scale(model, glm::vec3(100.0f, 100.0f, 1.0f));

	// TEXT STUFF BELOW --------------
	int error = FT_Init_FreeType(&library);
	S_ASSERT(!error, "Something went wrong")

	error = FT_New_Face(library, "../Resources/Fonts/Manrope-Regular.ttf", 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
		S_ASSERT(false, "Font file could be opened and read, but it appears that its font format is unsupported");
	}
	else if (error)
	{
		S_ASSERT(false, "The font file could not be opened or read, or maybe it is broken");
	}

	// Set font size
	error = FT_Set_Pixel_Sizes(face, 0, 24);
	S_ASSERT(!error, "Something went wrong")

	// Draw font glyphs
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RED,
		face->glyph->bitmap.width,
		face->glyph->bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		face->glyph->bitmap.buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	auto& charManager = CharacterManager::Instance();

	// load all glyphs
	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			S_ASSERT(false, "ERROR::FREETYTPE: Failed to load Glyph");
			continue;
		}
		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// generate texture
		unsigned int glyphTexture;
		glGenTextures(1, &glyphTexture);
		glBindTexture(GL_TEXTURE_2D, glyphTexture);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for later use
		Character character = { // texture
			glyphTexture,
			// size
			glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			// bearing
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			// advance
			(unsigned int)face->glyph->advance.x
		};

		charManager.AddCharacter(c, character);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	// Text buffer stuff
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);

	// Create a the buffer data for 2 positions and 2 texture coordinates
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

void TestingScene::Exit() {}

void TestingScene::Update(float deltaTime, const Services* services)
{
	// S_INFO(name)
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

	auto const& tex = CharacterManager::Instance().GetCharacter('A');

	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, tex.texture);

	// Draw the Quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// Unbind the opengl stuff
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	/// -------------------------------
	std::string text = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	float x          = 0.0f;
	float y          = 0.0f;
	float scale      = 1.0f;

	// Bind the font shader
	const Shader* fontShader = services->GetShaderManager()->Load("Shaders/font.vert").get();
	glUseProgram(fontShader->GetID());

	fontShader->setVec3("textColor", glm::vec3(1.0f, 0.8f, 0.3f));
	fontShader->setMat4("projection", glm::ortho(0.0f, width, 0.0f, height));

	glBindVertexArray(textVAO);

	// Iterate through all characters
	for (auto c : text)
	{
		Character ch = CharacterManager::Instance().GetCharacter(c);

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		// Update VBO for each character
		float vertices[6][4] = {
			{    xpos, ypos + h, 0.0, 0.0},
            {    xpos,     ypos, 0.0, 1.0},
            {xpos + w,     ypos, 1.0, 1.0},

			{    xpos, ypos + h, 0.0, 0.0},
            {xpos + w,     ypos, 1.0, 1.0},
            {xpos + w, ypos + h, 1.0, 0.0}
		};

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, ch.texture);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
