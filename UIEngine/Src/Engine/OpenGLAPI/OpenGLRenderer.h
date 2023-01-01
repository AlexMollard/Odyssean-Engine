#pragma once
#include "Engine/BaseRenderer.h"
class OpenGLRenderer : private BaseRenderer
{
public:
    OpenGLRenderer() = default;
	~OpenGLRenderer() = default;

    void Init(){};
	void Render(){};
	void Exit(){};

    // Cast to BaseRenderer
    operator BaseRenderer*()
    {
        return static_cast<BaseRenderer*>(this);
    }
};
