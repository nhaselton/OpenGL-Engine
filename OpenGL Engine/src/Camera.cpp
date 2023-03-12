#include <glm/glm.hpp>
#include <iostream>
#include "Camera.h"
#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


Camera::Camera() : Entity() {

}

Camera::Camera( glm::vec3 pos ) : Entity( pos ) {

}

Camera::Camera( Transform t ) : Entity( t ) {

}

glm::mat4 Camera::GetView() {
	glm::mat4 lookAt = glm::lookAt( transform.Position(), transform.Position() + GetForward(), glm::vec3( 0, 1, 0 ) );
	return lookAt;
}

glm::vec3 Camera::GetForward() {
	glm::vec3 direction;
	direction.x = cos( transform.Rotation().y ) * cos( transform.Rotation().x );
	direction.y = sin( transform.Rotation().x );
	direction.z = sin( transform.Rotation().y ) * cos( transform.Rotation().x );
	direction = glm::normalize( direction );
	return direction;
}