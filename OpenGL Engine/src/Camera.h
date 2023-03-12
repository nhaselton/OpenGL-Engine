#pragma once
#include <glm/glm.hpp>
#include "Entity.hpp"

class Camera : public Entity{
public:
	float pitch;
	float yaw;

	Camera();
	Camera( glm::vec3 pos );
	Camera( Transform t);
	
	glm::vec3 GetForward();
	glm::mat4 GetView();
};