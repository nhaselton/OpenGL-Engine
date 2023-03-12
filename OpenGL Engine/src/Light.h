#pragma once
#include <glm/glm.hpp>

enum LightType {
	LIGHT_POINT = 0,
	LIGHT_DIRECTIONAL = 1,
	LIGHT_SPOT = 2
};

struct Light {
	LightType lType;
	//All
	glm::vec3 color;
	//Directional
	glm::vec3 direction;
	//point
	glm::vec3 pos;
	float linear;
	float quadratic;
	//cutoff
	float cutoff;//12.5f
	float outerCutoff;//17.5f
};