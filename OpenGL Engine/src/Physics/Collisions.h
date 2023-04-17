#pragma once
#include <glm/glm.hpp>
#include <utility>
#include "Colliders.h"
#include "Simplex.h"

struct HitInfo {
	bool hit;
	glm::vec3 normal; 
	glm::vec3 normal2; 
	float depth;
};

struct ContinuousHitInfo {
	bool hit;
	float time;
};

//maybe don't need?
struct EPAFaces {
	std::vector<glm::vec4> normals;//normal XYZ,distance
	unsigned int minTri;
};

//given point P return the closest point on or in OBB b
glm::vec3			ClosestPtToOBB( glm::vec3 p, OBB& b );
//from given point p find how far it is from the nearest point of the OBB
float				DistancePointToOBBSquared( glm::vec3 p, OBB& b );
//from p (x) & q (y) find the 2 closest points c1 & c2, and their distances s & t, compare this against an epsilion to check for collisions returns squared distance between s & t
float				ClosestPointSegmentSegmentSquared( glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2, float& s, float& t, glm::vec3& c1, glm::vec3& c2 );
//Given OBB A & B return if there is intersections
bool				TestOBBOBB( OBB& a, OBB& b, HitInfo& h );
//Check if obb intersects obb
bool				TestOBBIntersectsPlane(OBB& a, Plane& p );
//Given 2 points of a segment, a,b and plane p return if they collide and give back normalized point t and point q
bool				TestLineIntersectsPlane( glm::vec3 a, glm::vec3 b, Plane p, float& t, glm::vec3& q );
//given 2 points check if it collides with an AABB
bool				TestLineIntersectsAABB( glm::vec3 p0, glm::vec3 p1, AABB& b );
//Covert				OBB to AABB and call test TestLineIntersectsAABB
bool				TestLineIntersectsOBB( glm::vec3 p0, glm::vec3 p1, OBB& b );
//					AABB collides AABB fast, only returns T/F
bool				TestAABBFast(const AABB& a, const AABB& b);
//					Tells Which Time 2 Moving AABBs Collide (if they collide)
ContinuousHitInfo	TestContinuousAABB( const AABB& a, const AABB& b, glm::vec3 velA, glm::vec3 velB );
//					Takes 2 colliders and decides if they're intersecting
HitInfo				GJK( const Collider& a, const Collider& b );
//					take two colliders and a direction and return the vertex on the Minkowski difference.
glm::vec3			GJKSupport( const Collider& a, const Collider& b, glm::vec3 direction );

bool				GJKNextSimplex( Simplex& points, glm::vec3& direction );
bool				GJKLine( Simplex& points, glm::vec3& direction );
bool				GJKTriangle( Simplex& points, glm::vec3& direction );
bool				GJKTetrahedron( Simplex& points, glm::vec3& direction );

// Gets hit normal & depth, should be able to dot with velocity to find info
void				EPA( const Collider& a, const Collider& b, Simplex simplex, HitInfo& hi );
//takes simplex from GJK and gives you all faces and normals from it
EPAFaces			EPAGetFaceNormals( const std::vector<glm::vec3>& polytope, const std::vector<size_t>& faces );
//tests if reverse edge already exsists in list and if so removes it
void				AddIfUniqueEdge( std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t a, size_t b );
