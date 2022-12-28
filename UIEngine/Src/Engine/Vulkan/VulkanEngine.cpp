#include "pch.h"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>

//the snake should move once per second, which is once
//every 1000 milliseconds
constexpr DWORD dwMillisecondsPerMove = 1000;

#include "VulkanEngine.h"

VulkanEngine::~VulkanEngine() {}

void VulkanEngine::Init(const char* windowName, int width, int height)
{
	m_close = false;
	std::cout << "Press Q to exit.";
}

float VulkanEngine::Update()
{
	INPUT_RECORD ir;
	HANDLE hConInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwReadCount;
	DWORD dwStartTimeOfMove = GetTickCount64();

	bool quit = false;

	//attempt to read input
	if (!ReadConsoleInput(hConInput, &ir, 1, &dwReadCount) || dwReadCount != 1)
		throw std::runtime_error("Unexpected input error!\n");

	if (ir.Event.KeyEvent.wVirtualKeyCode == 0x51)
		m_close = true;

	return 1;
}

void VulkanEngine::Render() {}

void* VulkanEngine::GetWindow()
{
	return nullptr;
}
