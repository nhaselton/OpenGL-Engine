#pragma once
#include <array>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Colliders.h"
#include "Collisions.h"
#include "Simplex.h"

std::vector<glm::vec3> penetrationAxes;
std::vector<float> penetrationAxesDistance;


void swapf( float& a, float& b ) {
	float temp = a;
	a = b;
	b = temp;
}

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
		else {
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

bool TestOBBOBB( OBB& a, OBB& b, HitInfo& h ) {
	float ra;
	float rb;
	float _t;
	glm::vec3 axes[15];
	float depths[15];
	glm::mat3 R( 1.0f ), absR( 1.0f );

	//compute the rotation matrix expressing b in a's coordinate space
	for ( int i = 0; i < 3; i++ )
		for ( int j = 0; j < 3; j++ )
			R[i][j] = glm::dot( a.u[i], b.u[j] );

	//translaiton vector
	glm::vec3 t = b.center - a.center;
	//translate into A's coord frame
	t = glm::vec3( glm::dot( t, a.u[0] ), glm::dot( t, a.u[1] ), glm::dot( t, a.u[2] ) );

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

		if ( fabs( t[i] ) > ra + rb ) return false;

		axes[i] = a.u[i];
		depths[i] = ra + rb - fabs( t[i] );//fabs(t[i]) - (ra + rb);//ra + rb - t[i];
	}


	// Test axes L = B0, L = B1, L = B2
	for ( int i = 0; i < 3; i++ ) {
		ra = a.e[0] * absR[0][i] + a.e[1] * absR[1][i] + a.e[2] * absR[2][i];
		rb = b.e[i];
		float _t = t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i];
		if ( fabs( _t ) > ra + rb ) return false;

		axes[i + 3] = b.u[i];
		depths[i + 3] = DEPTH;//ra + rb - t[i];
	}

	// L = A0 x B0
	ra = a.e[1] * absR[2][0] + a.e[2] * absR[1][0];
	rb = b.e[1] * absR[0][2] + b.e[2] * absR[0][1];
	_t = t[2] * R[1][0] - t[1] * R[2][0];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[6] = glm::cross( a.u[0], b.u[0] );
	depths[6] = DEPTH;

	// L = A0 x B1
	ra = a.e[1] * absR[2][1] + a.e[2] * absR[1][1];
	rb = b.e[0] * absR[0][2] + b.e[2] * absR[0][0];
	_t = t[2] * R[1][1] - t[1] * R[2][1];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[7] = glm::cross( a.u[0], b.u[1] );
	depths[7] = DEPTH;
	// L = A0 x B2
	ra = a.e[1] * absR[2][2] + a.e[2] * absR[1][2];
	rb = b.e[0] * absR[0][1] + b.e[1] * absR[0][0];
	_t = t[2] * R[1][2] - t[1] * R[2][2];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[8] = glm::cross( a.u[0], b.u[2] );
	depths[8] = DEPTH;
	// L = A1 x B0
	ra = a.e[0] * absR[2][0] + a.e[2] * absR[0][0];
	rb = b.e[1] * absR[1][2] + b.e[2] * absR[1][1];
	_t = t[0] * R[2][0] - t[2] * R[0][0];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[9] = glm::cross( a.u[1], b.u[0] );
	depths[9] = DEPTH;
	// L = A1 x B1
	ra = a.e[0] * absR[2][1] + a.e[2] * absR[0][1];
	rb = b.e[0] * absR[1][2] + b.e[2] * absR[1][0];
	_t = t[0] * R[2][1] - t[2] * R[0][1];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[10] = glm::cross( a.u[1], b.u[1] );
	depths[10] = DEPTH;
	// L = A1 x B2
	ra = a.e[0] * absR[2][2] + a.e[2] * absR[0][2];
	rb = b.e[0] * absR[1][1] + b.e[1] * absR[1][0];
	_t = t[0] * R[2][2] - t[2] * R[0][2];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[11] = glm::cross( a.u[1], b.u[2] );
	depths[11] = DEPTH;
	// L = A2 x B0
	ra = a.e[0] * absR[1][0] + a.e[1] * absR[0][0];
	rb = b.e[1] * absR[2][2] + b.e[2] * absR[2][1];
	_t = t[1] * R[0][0] - t[0] * R[1][0];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[12] = glm::cross( a.u[2], b.u[0] );
	depths[12] = DEPTH;
	// L = A2 x B1
	ra = a.e[0] * absR[1][1] + a.e[1] * absR[0][1];
	rb = b.e[0] * absR[2][2] + b.e[2] * absR[2][0];
	_t = t[1] * R[0][1] - t[0] * R[1][1];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[13] = glm::cross( a.u[2], b.u[1] );
	depths[13] = DEPTH;// L = A2 x B2
	ra = a.e[0] * absR[1][2] + a.e[1] * absR[0][2];
	rb = b.e[0] * absR[2][1] + b.e[1] * absR[2][0];
	_t = t[1] * R[0][2] - t[0] * R[1][2];
	if ( fabs( _t ) > ra + rb ) return false;
	axes[14] = glm::cross( a.u[2], b.u[2] );
	depths[14] = DEPTH;



	h.depth = FLT_MAX;
	for ( int i = 0; i < 15; i++ ) {
		if ( depths[i] < h.depth ) {
			if ( axes[i] != glm::vec3( 0 ) ) {
				h.depth = depths[i];
				h.normal = glm::normalize( axes[i] );

				if ( glm::dot( a.center - b.center, -h.normal ) > 0.0f )
					h.normal = -h.normal;
			}
		}
	}
	return true;
}

//Like SAT where you project
bool TestOBBIntersectsPlane( OBB& b, Plane& p ) {
	//calcultae projection
	float r = b.e[0] * fabs( glm::dot( p.n, b.u[0] ) ) +
		b.e[1] * fabs( glm::dot( p.n, b.u[1] ) ) +
		b.e[2] * fabs( glm::dot( p.n, b.u[2] ) );

	//get dist from center of box to plane
	float s = glm::dot( p.n, b.center ) - p.d;
	//intersection in range [-r,r]
	return abs( s ) <= r;
}

//todo fix divice by zero error
bool TestLineIntersectsPlane( glm::vec3 a, glm::vec3 b, Plane p, float& t, glm::vec3& q ) {
	//get T value for line
	glm::vec3 ab = b - a;
	t = ( p.d - glm::dot( p.n, a ) ) / glm::dot( p.n, ab );

	// if t [0,1] return point
	if ( t >= 0.0f && t <= 1.0f ) {
		q = a + t * ab;
		return true;
	}
	//else it didnt collide
	return false;
}

bool TestLineIntersectsAABB( glm::vec3 p0, glm::vec3 p1, AABB& b ) {
	glm::vec3 c = b.center;
	glm::vec3 e = b.e;
	glm::vec3 m = ( p0 + p1 ) * .5f;//center of line
	glm::vec3 d = p1 - m; // segment halflength
	m = m - c; // translate box and segment to center

	float adx = fabs( d.x );

	if ( fabs( m.x ) > e.x + adx )
		return false;

	float ady = fabs( d.y );
	if ( fabs( m.y ) > e.y + ady )
		return false;

	float adz = fabs( d.z );
	if ( fabs( m.z ) > e.z + adz )
		return 0;

	adx += FLT_EPSILON;
	ady += FLT_EPSILON;

	if ( fabs( m.y * d.z - m.z * d.y ) > e.y * adz + e.z * ady )
		return 0;

	if ( fabs( m.z * d.x - m.x * d.z ) > e.x * adz + e.z * adx )
		return 0;

	if ( fabs( m.x * d.y - m.y * d.x ) > e.x * ady + e.y * adx )
		return 0;

	//no seperating axis
	return true;
}

bool TestLineIntersectsOBB( glm::vec3 p0, glm::vec3 p1, OBB& b ) {
	glm::vec3 l = glm::normalize( p1 - p0 );
	glm::vec3 mid = ( p0 + p1 ) * .5f;
	float hl = glm::length( p0 + p1 );

	//Same as SAT
	glm::vec3 T = b.center - mid;
	glm::vec3 v;
	float r;

	if ( fabs( T.x ) > b.e.x + hl * fabs( l.x ) )
		return false;

	if ( fabs( T.y ) > b.e.y + hl * fabs( l.y ) )
		return false;

	if ( fabs( T.z ) > b.e.z + hl * fabs( l.z ) )
		return false;

	r = b.e.y * fabs( l.z ) + b.e.z * fabs( l.y );

	if ( fabs( T.y * l.z - T.z * l.y ) > r )
		return false;

	r = b.e.x * fabs( l.z ) + b.e.z * fabs( l.x );

	if ( fabs( T.z * l.x - T.x * l.z ) > r )
		return false;

	r = b.e.x * fabs( l.y ) + b.e.y * fabs( l.x );

	if ( fabs( T.x * l.y - T.y * l.x ) > r )
		return false;

	return true;
}

bool TestAABBFast( const AABB& a, const AABB& b ) {
	glm::vec3 minA = a.GetMin();
	glm::vec3 minB = b.GetMin();

	glm::vec3 maxA = a.GetMax();
	glm::vec3 maxB = b.GetMax();

	if ( maxA.x < minB.x || minA.x > maxB.x ) return false;
	if ( maxA.y < minB.y || minA.y > maxB.y ) return false;
	if ( maxA.z < minB.z || minA.z > maxB.z ) return false;

	return true;
}

inline float maxf( float a, float b ) {
	if ( a > b )
		return a;
	return b;
}

inline float minf( float a, float b ) {
	if ( a < b )
		return a;
	return b;
}


ContinuousHitInfo TestContinuousAABB( const AABB& a, const AABB& b, glm::vec3 velA, glm::vec3 velB ) {
	glm::vec3 minA = a.GetMin();
	glm::vec3 minB = b.GetMin();

	glm::vec3 maxA = a.GetMax();
	glm::vec3 maxB = b.GetMax();

	ContinuousHitInfo info;
	info.hit = false;
	info.time = 0;

	//check if already coliding
	//if ( maxA.x < minB.x || minA.x > maxB.x ) return info;
	//if ( maxA.y < minB.y || minA.y > maxB.y ) return info;
	//if ( maxA.z < minB.z || minA.z > maxB.z ) return info;

	glm::vec3 relVelocity = velB - velA;

	float first = 0.0f;
	float last = 1.0f;

	//deterime first & last contacts on all 3 axes
	for ( int i = 0; i < 3; i++ ) {
		if ( relVelocity[i] < 0.0f ) {

			if ( maxB[i] < minA[i] )
				return info;

			if ( maxA[i] < minB[i] )
				first = maxf( ( maxA[i] - minB[i] ) / relVelocity[i], first );

			if ( maxB[i] > minA[i] )
				last = minf( ( minA[i] - maxB[i] ) / relVelocity[i], last );
		}
		if ( relVelocity[i] > 0.0f ) {
			if ( minB[i] > maxA[i] )
				return info;

			if ( maxB[i] < minA[i] )
				first = maxf( ( minA[i] - maxB[i] ) / relVelocity[i], first );

			if ( maxA[i] > minB[i] )
				last = maxf( ( maxA[i] - minB[i] ) / relVelocity[i], last );
		}
	}
	//no overlap if first happens after last
	if ( first > last )
		return info;

	//did hit 
	info.hit = true;
	info.time = first;
	return info;
}



HitInfo	GJK( const Collider& a, const Collider& b ) {
	//initial support point
	glm::vec3 support = GJKSupport( a, b, glm::vec3( 1, 0, 0 ) );

	//Create the simplex of points
	Simplex points;
	points.push_front( support );
	glm::vec3 direction = -support;

	//keep adding until new point is not in front of search dir
	while ( true ) {
		support = GJKSupport( a, b, direction );

		//origin outside center
		if ( glm::dot( support, direction ) <= 0 ) {
			HitInfo hi;
			hi.hit = false;
			return hi;
		}

		points.push_front( support );

		if ( GJKNextSimplex( points, direction ) ) {
			HitInfo hi{ 0 };
			hi.hit = true;
			EPA( a, b, points, hi );
			return hi;
		}
	}
}

glm::vec3 GJKSupport( const Collider& a, const Collider& b, glm::vec3 direction ) {
	return a.FarthestPointInDirection( direction ) - b.FarthestPointInDirection( -direction );
}

bool GJKNextSimplex( Simplex& points, glm::vec3& direction ) {
	switch ( points.size() ) {
	case 2: return GJKLine( points, direction );
	case 3: return GJKTriangle( points, direction );
	case 4: return GJKTetrahedron( points, direction );
	default:
		std::cout << "BAD GJK SIMPLEX" << std::endl;
		return false;//SHOULD NOT HAPPEN
	}
}

bool SameDirection( const glm::vec3& a, const glm::vec3& b ) {
	return glm::dot( a, b ) > 0.0f;
}

bool GJKLine( Simplex& points, glm::vec3& direction ) {
	glm::vec3 a = points[0];
	glm::vec3 b = points[1];

	glm::vec3 ab = b - a;
	glm::vec3 ao = -a;

	if ( SameDirection( ab, ao ) ) {
		direction = glm::cross( glm::cross( ab, ao ),ab);
	}

	else {
		points = { a };
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

	glm::vec3 abc = glm::cross( ab, ac );//ab.cross( ac );

	if ( SameDirection( glm::cross(abc,ac),ao ) ) {
		if ( SameDirection( ac, ao ) ) {
			points = { a, c };
			direction = glm::cross( glm::cross( ac, ao ), ac );
		}
		else {
			return GJKLine( points = { a, b }, direction );
		}
	}
	else {
		if ( SameDirection( glm::cross( ab, abc ), ao ) ) {
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

void EPA( const Collider& a, const Collider& b, Simplex simplex, HitInfo& hi ) {
	std::vector<glm::vec3 > polytope( simplex.begin(), simplex.end() );
	//easier to index it like a mesh, this way you can get the faces and normals	
	std::vector<size_t>  faces = {
	0, 1, 2,
	0, 3, 1,
	0, 2, 3,
	1, 3, 2
	};

	EPAFaces epaFaces = EPAGetFaceNormals( polytope, faces );
	std::vector<glm::vec4>& normals = epaFaces.normals;
	unsigned int minFace = epaFaces.minTri;

	glm::vec3 minNormal = glm::vec3( 1.0 );
	float   minDistance = FLT_MAX;

	while ( minDistance == FLT_MAX ) {
		//normal = xyz, distance = w
		minNormal = glm::vec3( normals[minFace] );
		minDistance = normals[minFace].w;

		glm::vec3 support = GJKSupport( a, b, minNormal );
		float sDistance = glm::dot( minNormal, support );

		if ( abs( sDistance - minDistance ) > 0.001f ) {
			minDistance = FLT_MAX;

			//add vertex and repair faces when adding new points
			std::vector<std::pair<size_t, size_t>> uniqueEdges;
			for ( size_t i = 0; i < normals.size(); i++ ) {
				if ( SameDirection( normals[i], support ) ) {
					size_t f = i * 3;

					AddIfUniqueEdge( uniqueEdges, faces, f, f + 1 );
					AddIfUniqueEdge( uniqueEdges, faces, f + 1, f + 2 );
					AddIfUniqueEdge( uniqueEdges, faces, f + 2, f );

					faces[f + 2] = faces.back(); faces.pop_back();
					faces[f + 1] = faces.back(); faces.pop_back();
					faces[f] = faces.back(); faces.pop_back();

					normals[i] = normals.back(); normals.pop_back();

					i--;

					// caluclate the new faces normals
					std::vector<size_t> newFaces;
					for ( auto e : uniqueEdges ) {
						size_t edgeIndex1 = e.first;
						size_t edgeIndex2 = e.second;
						newFaces.push_back( edgeIndex1 );
						newFaces.push_back( edgeIndex2 );
						newFaces.push_back( polytope.size() );
					}

					polytope.push_back( support );

					EPAFaces newEPAFaces = EPAGetFaceNormals( polytope, faces );
					std::vector<glm::vec4>& newNormals = newEPAFaces.normals;
					unsigned int newMinFace = newEPAFaces.minTri;

					//find new cloest face
					float oldMinDistance = FLT_MAX;
					for ( size_t i = 0; i < normals.size(); i++ ) {
						if ( normals[i].w < oldMinDistance ) {
							oldMinDistance = normals[i].w;
							minFace = i;
						}
					}

					if ( newNormals[newMinFace].w < oldMinDistance ) {
						minFace = newMinFace + normals.size();
					}

					faces.insert( faces.end(), newFaces.begin(), newFaces.end() );
					normals.insert( normals.end(), newNormals.begin(), newNormals.end() );
				}
			}
		}
	}
	

	hi.normal = minNormal;
	hi.depth = minDistance + .001f;
	hi.hit = true;
}

EPAFaces EPAGetFaceNormals( const std::vector<glm::vec3>& polytope, const std::vector<size_t>& faces ) {
	std::vector<glm::vec4 > normals;
	size_t minTriangle = 0;
	float  minDistance = FLT_MAX;

	for ( size_t i = 0; i < faces.size(); i += 3 ) {
		glm::vec3 a = polytope[faces[i]];
		glm::vec3 b = polytope[faces[i + 1]];
		glm::vec3 c = polytope[faces[i + 2]];

		glm::vec3 normal = glm::cross( b - a, c - a );
		normal = glm::normalize( normal );
		float distance = glm::dot( normal, a );

		if ( distance < 0 ) {
			normal *= -1;
			distance *= -1;
		}

		normals.push_back( glm::vec4(normal, distance) );

		if ( distance < minDistance ) {
			minTriangle = i / 3;
			minDistance = distance;
		}
	}

	EPAFaces f;
	f.minTri = minTriangle;
	f.normals = normals;
	return f;
}

void AddIfUniqueEdge( std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t a, size_t b ) {
	auto reverse = std::find(               //      0--<--3
		edges.begin(),                     //     / \ B /   A: 2-0
		edges.end(),                       //    / A \ /    B: 0-2
		std::make_pair( faces[b], faces[a] ) //   1-->--2
	);

	if ( reverse != edges.end() ) {
		edges.erase( reverse );
	}

	else {
		edges.emplace_back( faces[a], faces[b] );
	}
}


