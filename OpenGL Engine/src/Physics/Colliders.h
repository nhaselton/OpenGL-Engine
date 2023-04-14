#pragma once
#include <iostream>

#include "glm/gtx/string_cast.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum ColliderType {
	COLLIDER_AABB,
	COLLIDER_OBB
};

class Plane {
public:
	glm::vec3	n;
	float		d;
};

class Collider {
public:
	ColliderType type;
};

class OBB : public Collider{
public:
	glm::vec3	center;
	glm::mat3	u; //rotation matrix
	glm::vec3	e; // pos halfwidth extents of oobb
public:
	glm::vec3 GetMin() const{
		return ( e * u) ;
	}

	glm::vec3 GetMax() const{
		return ( e * u) ;
	}
	
	glm::vec3 GetForward() {
		return glm::vec3( 0, 0, 1 ) * u;
	}

	glm::vec3 GetRight() {
		return glm::vec3( 1, 0, 0 ) * u;
	}

	glm::vec3 GetUp() {
		return glm::vec3( 0, 1, 0 ) * u;
	}
};

class AABB : public Collider {
public:
	glm::vec3 center;
	glm::vec3 e;
};	