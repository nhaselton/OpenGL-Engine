#include "Physics.h"
#include "RigidBody.h"
#include "glm/glm.hpp"

void Integrate( Entity& ent ) {
	RigidBody& rb = ent.rigidBody;
	Transform& t = ent.transform;

	glm::vec3 acceleration = rb.force / rb.mass;
	rb.velocity += acceleration;
	t.position += rb.velocity;
}