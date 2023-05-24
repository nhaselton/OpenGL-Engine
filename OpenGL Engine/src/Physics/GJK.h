#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <array>

struct Simplex {
public:
	Simplex() {
		points = { glm::vec3( 0 ),glm::vec3( 0 ),glm::vec3( 0 ),glm::vec3( 0 ) };
		size = 0;
	}

	Simplex& operator=( std::initializer_list<glm::vec3> list ) {
		for ( auto v = list.begin(); v != list.end(); v++ ) {
			points[std::distance( list.begin(), v )] = *v;
		}
		size = list.size();
		return *this;
	}

	void PushFront(glm::vec3 point) {
		points = { point , points[0], points[1], points[2] };
		size++;
		if ( size > 4 )
			size = 4;
	}

	glm::vec3& operator[]( unsigned i ) { return points[i]; }

	std::array<glm::vec3, 4> points;
	int size;

};

class Entity;

float GJK( Entity* a, Entity* b );//auto apply transforms and call noraml GJK
float GJK( std::vector<glm::vec3> a, std::vector<glm::vec3> b );
glm::vec3 GJKSupport(std::vector<glm::vec3> a, std::vector<glm::vec3> b, glm::vec3 dir);
glm::vec3 FindFarthestPoint( std::vector<glm::vec3> vertices, glm::vec3 dir );
bool  GJKNextSimplex(Simplex& simplex, glm::vec3& direction);
bool GJKLine( Simplex& simplex, glm::vec3& direction );
bool GJKTriangle( Simplex& simplex, glm::vec3& direction );
bool GJKTetrahedron( Simplex& simplex, glm::vec3& direction );
glm::vec3 ClosestPointOnSimplex( Simplex& points, glm::vec3& support );