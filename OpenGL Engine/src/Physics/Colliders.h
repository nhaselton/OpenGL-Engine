#pragma once
#include <iostream>
#include <vector>

#include "../Core/Transform.h"
#include "glm/gtx/string_cast.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Plane {
public:
	glm::vec3 n;
	float d;
};

enum ColliderType {
	COLLIDER_SPHERE = 1,
	COLLIDER_CAPSULE = 2,
	COLLIDER_HULL = 3,
};

class Collider {
public:
	ColliderType colliderType;
};

class Sphere : public Collider{
public:
	Sphere() {
		r = 1;
		c = glm::vec3( 0 );
		colliderType = COLLIDER_SPHERE;
	}
	Sphere(glm::vec3 c, float r) {
		this->r = r;
		this->c = c;
		colliderType = COLLIDER_SPHERE;
	}
	float r;
	glm::vec3 c;
};

class Capsule : public Collider {
public:
	Capsule() {
		c1 = glm::vec3( 0, 2, 0 );
		c2 = glm::vec3( 0, 0, 0 );
		float r = 0.5f;
		colliderType = COLLIDER_CAPSULE;
	}

	Capsule(glm::vec3 c1, glm::vec3 c2, float r) {
		this->c1 = c1; 
		this->c2 = c2; 
		this->r = r;
		colliderType = COLLIDER_CAPSULE;
	}

	glm::vec3 c1;
	glm::vec3 c2;
	float r;
	

};

class Hull : public Collider {
public:
	Hull() {
		colliderType = COLLIDER_HULL;
		drawMesh = nullptr;
		c = glm::vec3( 0 );
	}

	Hull( std::vector<glm::vec3> vertices, std::vector<unsigned short> indices );
	

	std::vector<glm::vec3> vertices;
	std::vector<unsigned short> indices;
	glm::vec3 c;

	//please god fix this 
	class Mesh* drawMesh;
};

struct HitInfo {
	int numContacts;
	float depth;
	glm::vec3 contactPoints[4];
	glm::vec3 normal;
};

Hull* CreateBoxHull(glm::vec3 offset, glm::vec3 scale);