#pragma once
#include "SceneStateMachine.h"
// This is the base class for all graphics API's

class BaseEngine
{
public:
	BaseEngine()  = default;
	~BaseEngine() = default;

	virtual void Initialize(const char* windowName, int width, int height) = 0;

	// Returns DT
	virtual float Update()
	{
		return -FLT_MAX;
	}
	virtual void  Render(){};

	virtual bool GetClose() const = 0;

private:
	bool m_close = true;
};
