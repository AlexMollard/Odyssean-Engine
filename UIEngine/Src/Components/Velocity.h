#pragma once
#include "glm/gtx/transform.hpp"

namespace components
{
// This is a component class. It is used to store data about the velocity of an entity.
// It should only contain data and no functions.
// It should be as small as possible.

class Velocity
{
public:
	float x = 0.0f;
	float y = 0.0f;

	// Default constructor
	Velocity() = default;

	Velocity(float x, float y, float z) : x(x), y(y) {}

	glm::vec2 GetVelocity() const { return {x, y}; }
};
} // namespace components