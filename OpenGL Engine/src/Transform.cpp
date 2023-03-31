#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Transform::Transform() {
	position = glm::vec3( 0 );
	rotation = glm::vec3( 0 );
	scale = glm::vec3( 1 );
	velocity = glm::vec3( 0 );
	rotationalVelocity = glm::vec3( 0 );
}
Transform::Transform( glm::vec3 pos ) {
	position = pos;
	rotation = glm::vec3( 0 );
	scale = glm::vec3( 1 );
	velocity = glm::vec3( 0 );
	rotationalVelocity = glm::vec3( 0 );
}

Transform::Transform( glm::vec3 pos, glm::vec3 rot, glm::vec3 sca ) {
	position = pos;
	rotation = rot;
	scale = sca;
	velocity = glm::vec3( 0 );
	rotationalVelocity = glm::vec3( 0 );
}

glm::mat4 Transform::Matrix() {
	glm::mat4 t = glm::translate( glm::mat4( 1.0 ), position );
	glm::mat4 r = glm::mat4(glm::quat( rotation ));
	glm::mat4 s = glm::scale( glm::mat4( 1.0 ), scale);
	return t * r * s;
}
