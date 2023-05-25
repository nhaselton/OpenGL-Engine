#include "RigidBody.h"

RigidBody::RigidBody() {
	mass = 1;
	velocity = glm::vec3( 0 );
	angularVelocity = glm::vec3( 0 );
	force = glm::vec3(0);
	useGravity = false;
}	

void RigidBody::AddForce(glm::vec3 force) {
	this->force += force;
}