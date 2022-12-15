#include "pch.h"

#include "SceneStateMachine.h"
#include "Services.h"
#include "TestingScene.h"

int main()
{
	// Memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::string windowName = "OpenGL UI Testing";

	// Create Window
	auto window = std::make_unique<Window>(1920, 1080, windowName);
	if (!window)
	{
		std::cout << "Failed to create Window!" << std::endl;
		return -1;
	}

	// Disable backface culling for testing
	glDisable(GL_CULL_FACE);

	auto shaderManager = ResourceManager<Shader>();
	shaderManager.Initialise("Shader Manager");

	// Load shaders
	shaderManager.Load("Shaders/lit.vert", "Shaders/lit.frag");
	shaderManager.Load("Shaders/font.vert", "Shaders/font.frag");

	Services services(window.get());
	services.SetShaderManager(&shaderManager);

	TestingScene scene("FirstScene");
	TestingScene Secondscene("SecondScene");

	auto stateMachine = SceneStateMachine(&services);
	stateMachine.AddScene(&scene);
	stateMachine.AddScene(&Secondscene);
	stateMachine.SetCurrentScene(&scene);

	//float timer = 0.0f;
	// Engine Loop
	while (!window->Window_shouldClose())
	{
		// Update Window
		window->Update_Window();
		float dt = window->GetDeltaTime();

		stateMachine.update(dt);
		stateMachine.render(*window);

		//timer += dt;
		//if (timer > 5.0f)
		//{
		//	stateMachine.SetCurrentScene(&Secondscene);
		//	timer = -5.0f;
		//}
		//else if (timer > 0.0f)
		//{
		//	stateMachine.SetCurrentScene(&scene);
		//}
	}

	return 0;
}