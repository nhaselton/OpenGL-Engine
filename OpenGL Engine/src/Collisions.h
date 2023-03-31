#pragma once
#include <glm/glm.hpp>

#include "OBB.h"

struct HitInfo {
	bool hit;
	glm::vec3 normal; 
	float depth;
};

//given point P return the closest point on or in OBB b
glm::vec3			ClosestPtToOBB( glm::vec3 p, OBB& b );
//from given point p find how far it is from the nearest point of the OBB
float				DistancePointToOBBSquared( glm::vec3 p, OBB& b );
//from p (x) & q (y) find the 2 closest points c1 & c2, and their distances s & t, compare this against an epsilion to check for collisions returns squared distance between s & t
float				ClosestPointSegmentSegmentSquared( glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2, float& s, float& t, glm::vec3& c1, glm::vec3& c2 );
//Given OBB A & B return if there is intersections
bool				TestOBBOBB( OBB& a, OBB& b );
//Given point a, b see if it intersects plane p, give back t and q
bool				TestLineInteresctAABB( glm::vec3 a, glm::vec3 b, Plane& p, float& t, glm::vec3& q);


bool				TestOBBOBB2( OBB& a, OBB& b,HitInfo& h );