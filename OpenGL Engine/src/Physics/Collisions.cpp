#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
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

glm::vec3 ClosestPointOnSegment( glm::vec3 a, glm::vec3 b, glm::vec3 c ) {
	glm::vec3 AB = b - a;
	float t = glm::dot( c - a, AB ) / dot( AB, AB );
	return a + minf( maxf( t, 0.0f ), 1.0f ) * AB;
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

HitInfo TestOBBOBB( Entity& entA, Entity& entB ) {
	HitInfo h{ 0 };
	float ra;
	float rb;
	float _t;
	glm::vec3 axes[15];
	float depths[15];
	glm::mat3 R( 1.0f ), absR( 1.0f );

	BoxCollider& a = entA.rigidBody.collider;
	BoxCollider& b = entB.rigidBody.collider;

	glm::mat3 aRotation = entA.rigidBody.collider.u * glm::mat3( glm::quat( entA.transform.rotation ) );
	glm::mat3 bRotation = entB.rigidBody.collider.u * glm::mat3( glm::quat( entB.transform.rotation ) );

	glm::vec3 aCenter = entA.transform.position + a.c;
	glm::vec3 bCenter = entB.transform.position + b.c;

	//compute the rotation matrix expressing b in a's coordinate space
	for ( int i = 0; i < 3; i++ )
		for ( int j = 0; j < 3; j++ )
			R[i][j] = glm::dot( aRotation[i], bRotation[j] );

	//translaiton vector
	glm::vec3 t = bCenter - aCenter;
	//translate into A's coord frame
	t = glm::vec3( glm::dot( t, aRotation[0] ), glm::dot( t, aRotation[1] ), glm::dot( t, aRotation[2] ) );

	//Compute common sub expressions, add epsilon to ged rid of NULL cross products on parallel lines
	for ( int i = 0; i < 3; i++ )
		for ( int j = 0; j < 3; j++ ) {
			absR[i][j] = fabs( R[i][j] ) + FLT_EPSILON;
		}

#define DEPTH  ra + rb - fabs(_t)//ra + rb - _t


	// Test axes L = A0, L = A1, L = A2
	for ( int i = 0; i < 3; i++ ) {
		ra = a.e[i];
		float rb = b.e[0] * absR[i][0] + b.e[1] * absR[i][1] + b.e[2] * absR[i][2];

		if ( fabs( t[i] ) > ra + rb ) return h;

		axes[i] = aRotation[i];
		depths[i] = ra + rb - fabs( t[i] );//fabs(t[i]) - (ra + rb);//ra + rb - t[i];
	}


	// Test axes L = B0, L = B1, L = B2
	for ( int i = 0; i < 3; i++ ) {
		ra = a.e[0] * absR[0][i] + a.e[1] * absR[1][i] + a.e[2] * absR[2][i];
		rb = b.e[i];
		float _t = t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i];
		if ( fabs( _t ) > ra + rb ) return h;

		axes[i + 3] = bRotation[i];
		depths[i + 3] = DEPTH;//ra + rb - t[i];
	}

	// L = A0 x B0
	ra = a.e[1] * absR[2][0] + a.e[2] * absR[1][0];
	rb = b.e[1] * absR[0][2] + b.e[2] * absR[0][1];
	_t = t[2] * R[1][0] - t[1] * R[2][0];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[6] = glm::cross( aRotation[0], bRotation[0] );
	depths[6] = DEPTH;

	// L = A0 x B1
	ra = a.e[1] * absR[2][1] + a.e[2] * absR[1][1];
	rb = b.e[0] * absR[0][2] + b.e[2] * absR[0][0];
	_t = t[2] * R[1][1] - t[1] * R[2][1];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[7] = glm::cross( aRotation[0], bRotation[1] );
	depths[7] = DEPTH;
	// L = A0 x B2
	ra = a.e[1] * absR[2][2] + a.e[2] * absR[1][2];
	rb = b.e[0] * absR[0][1] + b.e[1] * absR[0][0];
	_t = t[2] * R[1][2] - t[1] * R[2][2];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[8] = glm::cross( aRotation[0], bRotation[2] );
	depths[8] = DEPTH;
	// L = A1 x B0
	ra = a.e[0] * absR[2][0] + a.e[2] * absR[0][0];
	rb = b.e[1] * absR[1][2] + b.e[2] * absR[1][1];
	_t = t[0] * R[2][0] - t[2] * R[0][0];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[9] = glm::cross( aRotation[1], bRotation[0] );
	depths[9] = DEPTH;
	// L = A1 x B1
	ra = a.e[0] * absR[2][1] + a.e[2] * absR[0][1];
	rb = b.e[0] * absR[1][2] + b.e[2] * absR[1][0];
	_t = t[0] * R[2][1] - t[2] * R[0][1];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[10] = glm::cross( aRotation[1], bRotation[1] );
	depths[10] = DEPTH;
	// L = A1 x B2
	ra = a.e[0] * absR[2][2] + a.e[2] * absR[0][2];
	rb = b.e[0] * absR[1][1] + b.e[1] * absR[1][0];
	_t = t[0] * R[2][2] - t[2] * R[0][2];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[11] = glm::cross( aRotation[1], bRotation[2] );
	depths[11] = DEPTH;
	// L = A2 x B0
	ra = a.e[0] * absR[1][0] + a.e[1] * absR[0][0];
	rb = b.e[1] * absR[2][2] + b.e[2] * absR[2][1];
	_t = t[1] * R[0][0] - t[0] * R[1][0];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[12] = glm::cross( aRotation[2], bRotation[0] );
	depths[12] = DEPTH;
	// L = A2 x B1
	ra = a.e[0] * absR[1][1] + a.e[1] * absR[0][1];
	rb = b.e[0] * absR[2][2] + b.e[2] * absR[2][0];
	_t = t[1] * R[0][1] - t[0] * R[1][1];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[13] = glm::cross( aRotation[2], bRotation[1] );
	depths[13] = DEPTH;// L = A2 x B2
	ra = a.e[0] * absR[1][2] + a.e[1] * absR[0][2];
	rb = b.e[0] * absR[2][1] + b.e[1] * absR[2][0];
	_t = t[1] * R[0][2] - t[0] * R[1][2];
	if ( fabs( _t ) > ra + rb ) return h;
	axes[14] = glm::cross( aRotation[2], bRotation[2] );
	depths[14] = DEPTH;

	h.depth = FLT_MAX;
	for ( int i = 0; i < 15; i++ ) {
		if ( depths[i] < h.depth ) {
			if ( axes[i] != glm::vec3( 0 ) ) {
				h.depth = depths[i];
				h.normal = glm::normalize( axes[i] );

				if ( glm::dot( aCenter - bCenter, -h.normal ) > 0.0f )
					h.normal = -h.normal;
			}
		}
	}
	h.hit = true;
	return h;
}