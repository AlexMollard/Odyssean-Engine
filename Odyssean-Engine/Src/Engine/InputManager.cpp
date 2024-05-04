#include "InputManager.h"
#include <GLFW/glfw3.h>

bool InputManager::IsKeyHeld(Input::Keyboard key) const
{
	return m_KeyStates[static_cast<int>(key)] == KeyState::KEY_HOLD;
}

bool InputManager::IsKeyHeld(int key) const
{
	return m_KeyStates[key] == KeyState::KEY_HOLD;
}

bool InputManager::IsKeyReleased(Input::Keyboard key) const
{
	return m_KeyStates[static_cast<int>(key)] == KeyState::KEY_RELEASE;
}

bool InputManager::IsKeyReleased(int key) const
{
	return m_KeyStates[key] == KeyState::KEY_RELEASE;
}

InputManager& InputManager::GetInstance()
{
	static InputManager instance;
	return instance;
}

void InputManager::Init(GLFWwindow* window)
{
	m_Window = window;
}

bool InputManager::IsMouseButtonPressed(Input::Mouse button) const
{
	return glfwGetMouseButton(m_Window, static_cast<int>(button)) == GLFW_PRESS;
}

std::pair<float, float> InputManager::GetMousePosition() const
{
	double xPos;
	double yPos;
	glfwGetCursorPos(m_Window, &xPos, &yPos);
	return { static_cast<float>(xPos), static_cast<float>(yPos) };
}

void InputManager::Update()
{
	for (int i = 0; i < GLFW_KEY_LAST; i++)
	{
		int state              = glfwGetKey(m_Window, i);
		m_PreviousKeyStates[i] = m_CurrentKeyStates[i];
		m_CurrentKeyStates[i]  = static_cast<KeyState>(state);

		if (m_CurrentKeyStates[i] == KeyState::KEY_PRESS)
		{
			if (m_PreviousKeyStates[i] == KeyState::KEY_RELEASE)
			{
				m_KeyStates[i] = KeyState::KEY_PRESS;
			}
			else
			{
				m_KeyStates[i] = KeyState::KEY_HOLD;
			}
		}
		else
		{
			m_KeyStates[i] = KeyState::KEY_RELEASE;
		}

		if (s_Debug && (state || static_cast<int>(m_PreviousKeyStates[i])))
			std::cout << (char)i << ": " << (m_KeyStates[i] == KeyState::KEY_PRESS ? "Pressed" : m_KeyStates[i] == KeyState::KEY_HOLD ? "Held" : "Released") << std::endl;
	}

	if (s_Debug)
		DebugMouseOutput();
}

bool InputManager::IsKeyPressed(Input::Keyboard key) const
{
	return m_KeyStates[static_cast<int>(key)] == KeyState::KEY_PRESS;
}

bool InputManager::IsKeyPressed(int key) const
{
	return m_KeyStates[key] == KeyState::KEY_PRESS;
}

void InputManager::DebugMouseOutput() const
{
	// Output the mouse button states
	for (int i = 0; i < static_cast<int>(Input::Mouse::COUNT); i++)
	{
		int state = IsMouseButtonPressed(static_cast<Input::Mouse>(i));
		if (state)
		{
			// Get the enum name
			std::string name;
			switch (i)
			{
			case static_cast<int>(Input::Mouse::MACRO_4): name = "MACRO_4"; break;
			case static_cast<int>(Input::Mouse::MACRO_5): name = "MACRO_5"; break;
			case static_cast<int>(Input::Mouse::MACRO_6): name = "MACRO_6"; break;
			case static_cast<int>(Input::Mouse::MACRO_7): name = "MACRO_7"; break;
			case static_cast<int>(Input::Mouse::MACRO_8): name = "MACRO_8"; break;
			case static_cast<int>(Input::Mouse::LEFT): name = "LEFT"; break;
			case static_cast<int>(Input::Mouse::RIGHT): name = "RIGHT"; break;
			case static_cast<int>(Input::Mouse::MIDDLE): name = "MIDDLE"; break;
			default: name = "UNKNOWN"; break;
			}
			std::cout << name << ": " << (state ? "Pressed" : "Released") << std::endl;
		}
	}
}

InputManager::InputManager()
{
	std::memset(m_CurrentKeyStates, 0, sizeof(m_CurrentKeyStates));
	std::memset(m_PreviousKeyStates, 0, sizeof(m_PreviousKeyStates));
	std::memset(m_KeyStates, 0, sizeof(m_KeyStates));
}