#include "GJK.h"
#include "../Core/Entity.hpp"
#include "Collisions.h"
#include "glm/gtx/norm.hpp"

inline bool SameDirection(	const glm::vec3& direction, const glm::vec3& ao ) {
	return glm::dot( direction, ao ) > 0;
}

float GJK( Entity* a, Entity* b ) {
	std::vector<glm::vec3> aVerts;
	std::vector<glm::vec3> bVerts;
	Hull* hullA = ( Hull* ) a->rigidBody.collider;
	Hull* hullB = ( Hull* ) b->rigidBody.collider;

	for ( glm::vec3 p : hullA->vertices ) {
		aVerts.push_back( p + a->transform.position );
	}

	for ( glm::vec3 p : hullB->vertices ) {
		bVerts.push_back( p + b->transform.position );
	}

	return GJK( aVerts, bVerts );
}


float GJK( std::vector<glm::vec3> a, std::vector<glm::vec3> b ) {
	//inital support point (can be any dir)
	glm::vec3 support = GJKSupport( a, b, glm::vec3( 1, 0, 0 ) );
	Simplex points;
	points.PushFront( support );
	glm::vec3 direction = -support;
	while ( true ) {
		support = GJKSupport( a, b, direction );

		if ( glm::dot( support, direction ) <= 0 ) {
			return 0;
		}

		points.PushFront( support );

		if ( GJKNextSimplex( points, direction ) ) {
			return 1;
		}
	}

	return 42;
}

glm::vec3 ClosestPointOnSimplex( Simplex& points, glm::vec3& support ) {
#if 0
	switch ( points.size ) {
	case 1:
		return  points.points[0];
	case 2:
		return ClosestPointOnSegment( points[0], points[1] , support );
	case 3:
		return ClosestPointOnTriangle( points[0], points[1], points[2] , support );
	case 4:
		return ClosestPointOnTetraherdon( points[0], points[1], points[2], points[3], support );
	default:
		std::cout << "Closest point on simplex error! points: " << points.size << "\n";
		return glm::vec3( 0 );
	}
#endif
	return glm::vec3( 0 );
}


bool GJKNextSimplex( Simplex& points, glm::vec3& direction ) {
	switch ( points.size ) {
	case 2: return GJKLine( points,  direction ); break;
	case 3: return GJKTriangle( points,  direction ); break;
	case 4: return GJKTetrahedron( points, direction ); break;
	default:
		std::cout << "BAD GJK\n";
		return 0;//never hits here
	}
}


bool GJKLine( Simplex& simplex, glm::vec3& direction ) {
	glm::vec3 a = simplex[0];
	glm::vec3 b = simplex[1];

	glm::vec3 ab = b - a;
	glm::vec3 ao = -a;

	if ( SameDirection( ab, ao ) ) {
		direction = glm::cross( glm::cross( ab, ao ), ab );
	}
	else {
		simplex = { a };
		direction = ao;
	}

	return false;
}

bool GJKTriangle( Simplex& points, glm::vec3& direction ) {
	glm::vec3 a = points[0];
	glm::vec3 b = points[1];
	glm::vec3 c = points[2];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ao = -a;

	glm::vec3 abc = glm::cross( ab, ac );

	if ( SameDirection( glm::cross(abc,ac), ao )) {
		if ( SameDirection( ac, ao ) ) {
			points = { a, c };
			direction = glm::cross( glm::cross( ac, ao ), ac );
		}

		else {
			return GJKLine( points = { a, b }, direction );
		}
	}
	else {
		if ( SameDirection( glm::cross(ab,abc), ao ) ) {
			return GJKLine( points = { a, b }, direction );
		}
		else {
			if ( SameDirection( abc, ao ) ) {
				direction = abc;
			}

			else {
				points = { a, c, b };
				direction = -abc;
			}
		}
	}
	return false;
}

bool GJKTetrahedron( Simplex& points, glm::vec3& direction ) {
	glm::vec3 a = points[0];
	glm::vec3 b = points[1];
	glm::vec3 c = points[2];
	glm::vec3 d = points[3];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ad = d - a;
	glm::vec3 ao = -a;

	glm::vec3 abc = glm::cross( ab, ac );
	glm::vec3 acd = glm::cross( ac, ad );
	glm::vec3 adb = glm::cross( ad, ab );

	if ( SameDirection( abc, ao ) ) {
		return GJKTriangle( points = { a, b, c }, direction );
	}

	if ( SameDirection( acd, ao ) ) {
		return GJKTriangle( points = { a, c, d }, direction );
	}

	if ( SameDirection( adb, ao ) ) {
		return GJKTriangle( points = { a, d, b }, direction );
	}

	return true;
}


glm::vec3 FindFarthestPoint( std::vector<glm::vec3> vertices, glm::vec3 dir ) {

	int far = 0;//index
	float maxDist = -FLT_MAX;
	for ( int i = 0; i < vertices.size(); i++ ) {
		float dist = glm::dot( vertices[i], dir );
		if ( dist > maxDist ) {
			maxDist = dist;
			far = i;
		}
	}

	return vertices[far];
}

glm::vec3 GJKSupport( std::vector<glm::vec3> a, std::vector<glm::vec3> b, glm::vec3 dir ) {
	return FindFarthestPoint( a, dir ) - FindFarthestPoint( b, -dir );
}




