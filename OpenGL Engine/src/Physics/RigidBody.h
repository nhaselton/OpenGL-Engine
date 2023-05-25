#pragma once
#include <glm/glm.hpp>
#include "Colliders.h"

class RigidBody {
public:
	RigidBody();
	glm::vec3	velocity;
	glm::vec3	angularVelocity;
	float		mass;
	glm::vec3	force;
	BoxCollider	collider;
	bool		useGravity;
public:
	void		AddForce(glm::vec3 force);
};