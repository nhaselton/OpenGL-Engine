#pragma once
#include "Colliders.h"
#include "../Core/Entity.hpp"

struct HitInfo {
	bool hit;
	glm::vec3 normal;
	float depth;
};

// from point t what is the closest point on the line p1->p2
glm::vec3 ClosestPointOnSegment( glm::vec3 a, glm::vec3 b, glm::vec3 c );
glm::vec3 ClosestPointOnTriangle( glm::vec3 l1, glm::vec3 l2, glm::vec3 l3, glm::vec3 p );
HitInfo TestOBBOBB( Entity& entA, Entity& entB );
