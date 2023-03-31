#pragma once
#include "OBB.h"
#include "Collisions.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>


static float clampf( float f, float a, float b ) {
	if ( f > b )
		return b;
	if ( f < a )
		return a;
	return f;
}

glm::vec3 ClosestPtToOBB( glm::vec3 p, OBB& b ) {
	glm::vec3 d = p - b.center;
	
	//result starts at center of box, then moves from there
	glm::vec3 q = b.center;
	
	//for each axis
	for ( int i = 0; i < 3; i++ ) {
		//project D to axis and get distance
		float dist = glm::dot( d, b.u[i] );
		
		//clamp dist to box
		if ( dist > b.e[i] ) dist = b.e[i];
		if ( dist < -b.e[i] ) dist = -b.e[i];
		//step in that direction to get the world coordinate
		q += dist * b.u[i];
	}
	return q;
}

float DistancePointToOBBSquared( glm::vec3 p, OBB& b ) {
	glm::vec3 closetPt = ClosestPtToOBB( p, b );
	float sqDist = glm::dot( closetPt - p, closetPt - p );
	return sqDist;
}

float ClosestPointSegmentSegmentSquared( glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2, float& s, float& t, glm::vec3& c1, glm::vec3& c2 ) {
	glm::vec3 d1 = q1 - p1;
	glm::vec3 d2 = q2 - p2;

	glm::vec3 r = p1 - p2;
	float a = glm::dot( d1, d1 ); //squared distance
	float e = glm::dot( d2, d2 );
	float f = glm::dot( d2, r );

	//check if segmetns degen into points
	if ( a <= FLT_EPSILON && e <= FLT_EPSILON ) {
		//they are both on the line
		s = 0.0f;
		t = 0.0f;

		c1 = p1;
		c2 = p2;

		return glm::dot( c1 - c2, c1 - c2 );
	}
	// check if hte first element is a point
	if ( a < FLT_EPSILON ) {
		s = 0.0f;
		t = f / e;
		t = clampf( t, 0.0f, 1.0f );
	}
	else {
		float c = glm::dot( d1, r );
		//second segment
		if ( e < FLT_EPSILON ) {
			t = 0.0f;
			s = clampf( -c / a, 0.0f, 1.0f );
		}//neither do
		else
		{
			float b = glm::dot( d1, d2 );
			float denom = a * e - b * b;
			
			//if segments are not parallel
			if ( denom != 0.0f ) {
				s = clampf( ( b * f - c * e ) / denom, 0.0f, 1.0f );
			}
			else
				s = 0.0f;

			//compute closest point on L2 to S1
			t = ( b * s + f ) / e;

			//if t in [0,1] done, else clamp and recalculate s
			if ( t < 0.0f ) {
				t = 0.0f;
				s = clampf( -c / a, 0.0f, 1.0f );
			}
			else if ( t > 1.0f ) {
				t = 1.0f;
				s = clampf( ( b - c / a ), 0.0f, 1.0f );
			}
		}
	}
	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	return glm::dot( c1 - c2, c1 - c2 );
}

//SAT Notes
/*
	if there is a space to where a plane can seperate the two convex shapes without touching them they MUST be seperated
	Axis to test:
		- Parallel to face normals of A
		- Parallel to face normals of B
		- Parallel to vector from cross products of all edges in A with all edges in B
		2F + E^2 complexity, not for mildly+ compilcaited shapes
		Possible speedup = cache last axis and use if not rotated to check first hoping for quick check
		pg 159 for complexity
*/

//pg 102
bool TestOBBOBB( OBB& a, OBB& b ) {
	float ra;
	float rb;
	glm::mat3 R(1.0f), absR(1.0f);
	
	//compute the rotation matrix expressing b in a's coordinate space
	for ( int i = 0; i < 3; i++ )
		for ( int j = 0; j < 3; j++ ) 
			R[i][j] = glm::dot( a.u[i], b.u[j] );
	
	//translaiton vector
	glm::vec3 t = b.center - a.center;
	//translate into A's coord frame
	t = glm::vec3( glm::dot( t, a.u[0] ), glm::dot( t,a.u[1] ), glm::dot( t, a.u[2] ) );
	
	//Compute common sub expressions, add epsilon to ged rid of NULL cross products on parallel lines
	for ( int i = 0; i < 3 ; i++ )
		for ( int j = 0; j < 3; j++ ) {
			absR[i][j] = fabs( R[i][j] ) + FLT_EPSILON;
		}

	// Test axes L = A0, L = A1, L = A2
	for ( int i = 0; i < 3; i++ ) {
		ra = a.e[i];
		rb = b.e[0] * absR[i][0] + b.e[1] * absR[i][1] + b.e[2] * absR[i][2];
		if ( fabs( t[i] ) > ra + rb ) return false;
	}
	// Test axes L = B0, L = B1, L = B2
	for ( int i = 0; i < 3; i++ ) {
		ra = a.e[0] * absR[0][i] + a.e[1] * absR[1][i] + a.e[2] * absR[2][i];
		rb = b.e[i];
		if ( fabs( t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i] ) > ra + rb ) return false;
	}

	// L = A0 x B0
	ra = a.e[1] * absR[2][0] + a.e[2] * absR[1][0];
	rb = b.e[1] * absR[0][2] + b.e[2] * absR[0][1];
	if ( fabs( t[2] * R[1][0] - t[1] * R[2][0] ) > ra + rb ) return false;
	// L = A0 x B1
	ra = a.e[1] * absR[2][1] + a.e[2] * absR[1][1];
	rb = b.e[0] * absR[0][2] + b.e[2] * absR[0][0];
	if ( fabs( t[2] * R[1][1] - t[1] * R[2][1] ) > ra + rb ) return false;
	// L = A0 x B2
	ra = a.e[1] * absR[2][2] + a.e[2] * absR[1][2];
	rb = b.e[0] * absR[0][1] + b.e[1] * absR[0][0];
	if ( fabs( t[2] * R[1][2] - t[1] * R[2][2] ) > ra + rb ) return false;
	// L = A1 x B0
	ra = a.e[0] * absR[2][0] + a.e[2] * absR[0][0];
	rb = b.e[1] * absR[1][2] + b.e[2] * absR[1][1];
	if ( fabs( t[0] * R[2][0] - t[2] * R[0][0] ) > ra + rb ) return false;
	// L = A1 x B1
	ra = a.e[0] * absR[2][1] + a.e[2] * absR[0][1];
	rb = b.e[0] * absR[1][2] + b.e[2] * absR[1][0];
	if ( fabs( t[0] * R[2][1] - t[2] * R[0][1] ) > ra + rb ) return false;
	// L = A1 x B2
	ra = a.e[0] * absR[2][2] + a.e[2] * absR[0][2];
	rb = b.e[0] * absR[1][1] + b.e[1] * absR[1][0];
	if ( fabs( t[0] * R[2][2] - t[2] * R[0][2] ) > ra + rb ) return false;
	// L = A2 x B0
	ra = a.e[0] * absR[1][0] + a.e[1] * absR[0][0];
	rb = b.e[1] * absR[2][2] + b.e[2] * absR[2][1];
	if ( fabs( t[1] * R[0][0] - t[0] * R[1][0] ) > ra + rb ) return false;
	// L = A2 x B1
	ra = a.e[0] * absR[1][1] + a.e[1] * absR[0][1];
	rb = b.e[0] * absR[2][2] + b.e[2] * absR[2][0];
	if ( fabs( t[1] * R[0][1] - t[0] * R[1][1] ) > ra + rb ) return false;
	// L = A2 x B2
	ra = a.e[0] * absR[1][2] + a.e[1] * absR[0][2];
	rb = b.e[0] * absR[2][1] + b.e[1] * absR[2][0];
	if ( fabs( t[1] * R[0][2] - t[0] * R[1][2] ) > ra + rb ) return false;
	return true;


}

//Test OBB and give back hit info
bool TestOBBOBB2( OBB& a, OBB& b , HitInfo& h) {
	float minDepth = 0.0f;
	glm::vec3 depthNormal = glm::vec3( 0 );

	glm::vec3* aAxes = a.GetAxes();
	glm::vec3* bAxes = b.GetAxes();

	glm::vec3 allAxes[] = {
			aAxes[0],
			aAxes[1],
			aAxes[2],
			bAxes[0],
			bAxes[1],
			bAxes[2],
			glm::cross( aAxes[0], bAxes[0] ),
			glm::cross( aAxes[0], bAxes[1] ),
			glm::cross( aAxes[0], bAxes[2] ),
			glm::cross( aAxes[1], bAxes[0] ),
			glm::cross( aAxes[1], bAxes[1] ),
			glm::cross( aAxes[1], bAxes[2] ),
			glm::cross( aAxes[2], bAxes[0] ),
			glm::cross( aAxes[2], bAxes[1] ),
			glm::cross( aAxes[2], bAxes[2] )
	};

	glm::vec3* aVerts = a.GetVertices();
	glm::vec3* bVerts = b.GetVertices();


}
