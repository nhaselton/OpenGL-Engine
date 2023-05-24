#pragma once
#include "Colliders.h"
#include "GJK.h" // auto include this when including collisions
#include "../core/Entity.hpp"

//figure out what function to call from 2 generic colliders
HitInfo CheckCollision( Entity* a, Entity* b );
HitInfo SphereCollideSphere( Entity* a, Entity* b );
HitInfo SphereCollideCapsule( Entity* a, Entity* b );
HitInfo SphereCollideHull( Entity* sp, Entity* hl );


// from point t what is the closest point on the line p1->p2
glm::vec3 ClosestPointOnSegment( glm::vec3 a, glm::vec3 b, glm::vec3 c );
glm::vec3 ClosestPointOnHull( Hull* hull, glm::vec3 hullOffset, glm::vec3 p );
glm::vec3 ClosestPointOnTriangle( glm::vec3 l1, glm::vec3 l2, glm::vec3 l3, glm::vec3 p );
glm::vec3 ClosestPointOnTetraherdon( glm::vec3 l1, glm::vec3 l2, glm::vec3 l3, glm::vec3 l4, glm::vec3 p );