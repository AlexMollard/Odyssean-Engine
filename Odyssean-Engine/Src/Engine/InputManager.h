#pragma once
#include "Input.h"
#include <iostream>
#include <unordered_set>

struct GLFWwindow;

const static bool s_Debug = false;

enum class KeyState
{
	KEY_RELEASE,
	KEY_PRESS,
	KEY_HOLD,
	COUNT
};

class InputManager
{
public:
	static InputManager& GetInstance();

	void Init(GLFWwindow* window);

	bool IsMouseButtonPressed(Input::Mouse button) const;
	std::pair<float, float> GetMousePosition() const;

	void Update();

	// Int overloads
	bool IsKeyPressed(int key) const;
	bool IsKeyHeld(int key) const;
	bool IsKeyReleased(int key) const;

	// Keyboard overloads
	bool IsKeyPressed(Input::Keyboard key) const;
	bool IsKeyHeld(Input::Keyboard key) const;
	bool IsKeyReleased(Input::Keyboard key) const;

private:
	void DebugMouseOutput() const;

	GLFWwindow* m_Window{};
	KeyState m_CurrentKeyStates[348]{};
	KeyState m_PreviousKeyStates[348]{};
	KeyState m_KeyStates[348]{};

	InputManager();

	~InputManager() = default;
};
