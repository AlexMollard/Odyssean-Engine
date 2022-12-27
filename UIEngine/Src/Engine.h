#pragma once

namespace UIEngine
{

class Engine
{
public:
	Engine() = default;
	~Engine();

	void Init(const char* windowName, int width, int height);

	// Returns DT
	float Update();
	void Render();

	bool GetClose() const { return m_close; }

	void* GetWindow();

private:
	bool m_close = false;
};
} // namespace UIEngine