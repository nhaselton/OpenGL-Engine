#pragma once
#include <glm/glm.hpp>

class RigidBody {
public:
	RigidBody();
	glm::vec3	velocity;
	glm::vec3	angularVelocity;
	float		mass;
	glm::vec3	force;

	bool		useGravity;
public:
	void		AddForce(glm::vec3 force);
	void		Integrate();
};