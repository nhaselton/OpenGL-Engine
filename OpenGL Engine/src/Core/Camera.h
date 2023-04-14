#pragma once
#include "Entity.hpp"
#include <glm/glm.hpp>

class Camera : public Entity{
public:
	float pitch;
	float yaw;

	Camera();
	Camera( glm::vec3 pos );
	Camera( Transform t);
	
	glm::vec3 GetForward();
	glm::mat4 GetView();
	glm::mat4 GetInterpolatedView( float percent );
};