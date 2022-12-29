#pragma once
// This is the base class for all graphics API's

class BaseEngine
{
public:
	BaseEngine()  = default;
	~BaseEngine() = default;

	virtual void Init(const char* windowName, int width, int height) = 0;

	// Returns DT
	virtual float Update() = 0;
	virtual void Render()  = 0;

	virtual bool GetClose() const = 0;

private:
	bool m_close = true;
};
