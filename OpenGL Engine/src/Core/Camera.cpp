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

glm::mat4 Camera::GetInterpolatedView( float percent ) {
	glm::vec3 posNow = transform.Position();
	glm::vec3 posPrev = transform.Position() - rigidBody.velocity;
	glm::vec3 _pos = posNow * percent + posPrev* ( 1.0f - percent );

	//TODO fix rotations and use quat for angular velocity
	//glm::quat rotNow = glm::quat(transform.Rotation());
	//glm::quat rotPrev = transform.Rotation() - transform.RotationalVelocity();
	//glm::quat _rotQ = glm::slerp(rotPrev,rotNow, percent);
	//glm::vec3 _rot = glm::eulerAngles( _rotQ );
	
	glm::vec3 rotNow = transform.Rotation();
	glm::vec3 rotPrev = transform.Rotation() - rigidBody.angularVelocity;
	glm::vec3 _rot = ( rotNow ) *percent + rotPrev * ( 1.0f - percent );
	//glm::vec3 _rot = transform.Rotation();

	// Get Fowrard
	glm::vec3 direction;
	direction.x = cos( _rot.y ) * cos( _rot.x );
	direction.y = sin( _rot.x );
	direction.z = sin( _rot.y ) * cos( _rot.x );
	direction = glm::normalize( direction );

	// Get View
	glm::mat4 lookAt = glm::lookAt( _pos, _pos + direction, glm::vec3( 0, 1, 0 ) );
	return lookAt;
}