#pragma once
#include <glm/glm.hpp>

class RigidBody {
public:
	glm::vec3	velocity;
	glm::vec3	angularVelocity;
	float		mass;

	bool		useGravity;
public:
	void		AddForce();

};