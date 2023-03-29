#pragma once
#include "glm/glm.hpp"

class OBB {
public:
	glm::vec3 center;
	glm::mat3 u; //rotation matrix
	glm::vec3 e; // pos halfwidth extents of oobb
};