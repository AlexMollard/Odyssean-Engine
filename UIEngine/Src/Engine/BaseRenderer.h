#pragma once

class BaseRenderer
{
public:
	virtual void Init()   = 0;
	virtual void Render() = 0;
	virtual void Exit()   = 0;
};