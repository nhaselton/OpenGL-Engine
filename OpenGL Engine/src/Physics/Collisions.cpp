#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "RigidBody.h"
#include "Colliders.h"
#include "Collisions.h"
#include "../core/Entity.hpp"

float minf( float a, float b ) {
	if ( a > b )
		return b;
	return a;
}
float maxf( float a, float b ) {
	if ( a > b )
		return a;
	return b;
}

//todo there is a better way than nested if elses
HitInfo CheckCollision( Entity* ra, Entity* rb) {
	Collider* a = ra->rigidBody.collider;
	Collider* b = rb->rigidBody.collider;
	if ( a == nullptr || b == nullptr ) {
		std::cout << "Null collider";
		HitInfo h{ 0 };
		return h;
	}

	return SphereCollideCapsule( ra, rb );

	if ( a->colliderType == COLLIDER_SPHERE ) {
		if ( b->colliderType == COLLIDER_SPHERE )
			return SphereCollideSphere( ra, rb );
		
		if ( b->colliderType == COLLIDER_CAPSULE)
			return SphereCollideCapsule( ra, rb );
	}

	if ( b->colliderType == COLLIDER_SPHERE ) {
		if ( a->colliderType == COLLIDER_SPHERE )
			return SphereCollideSphere( rb, ra );

		if ( a->colliderType == COLLIDER_CAPSULE )
			return SphereCollideCapsule( rb, ra );
	}

}


HitInfo SphereCollideSphere( Entity* e1, Entity* e2 ) {
	HitInfo hi{ 0 };

	Sphere* a = (Sphere*) e1->rigidBody.collider;
	Sphere* b = (Sphere*) e2->rigidBody.collider;
	

	//|C2-C1| - r1 - r2 <= 0
	glm::vec3 c1 = e1->transform.position + a->c;
	glm::vec3 c2 = e2->transform.position + b->c ;
	float distance = glm::length( c2 - c1 ) - a->r - b->r;

	//todo figure out how to make middle of 2 surface points (not center) slide 40
	if ( distance <= 0) {
		hi.numContacts = 1;
		hi.contactPoints[0] = c2 - c1;
		hi.normal = glm::normalize( c2 - c1 );
		hi.depth = distance;
	}

	return hi;
}


//TODO REST OF THIS
HitInfo SphereCollideCapsule( Entity* s, Entity* c ) {
	HitInfo hi{ 0 };
	Sphere* sphere = ( Sphere* ) s->rigidBody.collider;
	Capsule* capsule = ( Capsule* ) c->rigidBody.collider;

	glm::vec3 c1 = capsule->c1 + c->transform.position;
	glm::vec3 c2 = capsule->c2 + c->transform.position;

	//Top
	glm::vec3 cS = s->transform.position + sphere->c;
	glm::vec3 L = ClosestPointOnSegment( c1, c2, cS );
	
	float distance = glm::length( cS - L ) - sphere->r - capsule->r;

	if ( distance <= 0 ) {
		hi.numContacts = 1;
		hi.depth = distance;
		hi.normal = ( glm::normalize( cS - L ) );
		hi.contactPoints[0] = ( cS - L );
	}
	

	return hi;
}	

glm::vec3 ClosestPointOnSegment( glm::vec3 a, glm::vec3 b, glm::vec3 c ) {
	glm::vec3 AB = b - a;
	float t = glm::dot( c - a, AB ) / dot( AB, AB );
	return a + minf( maxf( t, 0.0f ), 1.0f ) * AB;
}

HitInfo SphereCollideHull( Entity* sp, Entity* hl ) {
	HitInfo hi{ 0 };
	
	Hull* h = ( Hull* ) sp->rigidBody.collider;
	Sphere* s = ( Sphere* ) hl->rigidBody.collider;
	glm::vec3 spherePos = s->c + sp->transform.position;
	
	glm::vec3 L = ClosestPointOnHull( h, hl->transform.position , spherePos );
	float offset = glm::length( spherePos - L ) - s->r;

	if ( offset <= 0 ) {
		hi.numContacts = 1;
	}
	return hi;
}

float DistPointSegmentSq( glm::vec3 a, glm::vec3 b, glm::vec3 c ) {
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;
	float e = glm::dot( ac, ab );
	if ( e <= 0.0f ) 
		return dot( ac, ac );

	float f = glm::dot( ab, ab );

	if ( e > f )
		return glm::dot( bc, bc );

	return glm::dot( ac, ac ) - e * e / f;
}

glm::vec3 ClosestPointOnHull( Hull* hull,glm::vec3 hullOffset, glm::vec3 p ) {
	//For each face
	std::vector<unsigned short>& ind = hull->indices;

	float shortestDist = FLT_MAX;
	glm::vec3 l1, l2, l3;
	for ( int i = 0 ;i < hull->indices.size(); i+=3 ){
		//check 3 lines of face for closest distance to point, record closest to do calculations on after	
		glm::vec3 v1 = hull->vertices[hull->indices[i]] + hull->c + hullOffset;
		glm::vec3 v2 = hull->vertices[hull->indices[i+1]] + hull->c + hullOffset;
		glm::vec3 v3 = hull->vertices[hull->indices[i+2]] + hull->c + hullOffset;
		float a = DistPointSegmentSq( v1 , v2 , p );
		float b = DistPointSegmentSq( v2 , v3 , p );
		float c = DistPointSegmentSq( v3 , v1 , p );
		
		if ( a < shortestDist ) {
			shortestDist = a;
			l1 = v1;
			l2 = v2;
			l3 = v3;
		}

		if ( b < shortestDist ) {
			shortestDist = b;
			l1 = v1;
			l2 = v2;
			l3 = v3;
		}

		if ( c < shortestDist ) {
			shortestDist = c;
			l1 = v1;
			l2 = v2;
			l3 = v3;
		}
	}

	glm::vec3 closestPoint = ClosestPointOnTriangle( l1, l2, l3, p );

	return closestPoint;
}


glm::vec3 ClosestPointOnTriangle( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 p ) {
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ap = p - a;
	//p outside a
	float d1 = glm::dot( ab, ap );
	float d2 = glm::dot( ac, ap );
	if ( d1 <= 0.0f && d2 <= 0.0f ) return a;

	//p outside b
	glm::vec3 bp = p - b;
	float d3 = glm::dot( ab, bp );
	float d4 = glm::dot( ac, bp );
	if ( d3 >= 0.0f && d4 <= d3 ) return b;

	//p in Ab
	float vc = d1 * d4 - d3 * d2;
	if ( vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f ) {
		float v = d1 / ( d1 - d3);
		return ( a + v * ab );
	}

	//p outside c
	glm::vec3 cp = p - c;
	float d5 = glm::dot( ab, cp );
	float d6 = glm::dot( ac, cp );
	if ( d6 >= 0.0f && d5 <= d6 )
		return c;

	//if p in AC
	float vb = d5 * d2 - d1 * d6;
	if ( vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f ) {
		float w = d2 / ( d2 - d6 );
		return a + w * ac;
	}

	//if p in bc
	float va = d3 * d6 - d5 * d4;
	if ( va <= 0.0f && ( d4 - d3 ) >= 0.0f && ( d5 - d6 ) >= 0.0f ) {
		float w = ( d4 - d3 ) / ( ( d4 - d3 ) + ( d5 - d6 ) );
		return b + w * ( c - b );
	}

	// p inside face region
	float denom = 1.0f / ( va + vb + vc );
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w;
}

glm::vec3 ClosestPointOnTetraherdon( glm::vec3 l1, glm::vec3 l2, glm::vec3 l3, glm::vec3 l4, glm::vec3 p ) {
	std::cout << "[ERROR]havent implemented closestpointontetrahedron";

	return glm::vec3(-1);
}