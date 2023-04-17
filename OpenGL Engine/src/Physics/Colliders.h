#pragma once
#include <iostream>
#include <vector>

#include "glm/gtx/string_cast.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum ColliderType {
	COLLIDER_AABB,
	COLLIDER_OBB
};

class Plane {
public:
	glm::vec3	n;
	float		d;
};

class Collider {
public:
	ColliderType type;
	virtual std::vector<glm::vec3> GetVertices() const = 0; //probably should cache but dont want to constantly recalc (even though it probably will need to every time it moves)

	glm::vec3 FarthestPointInDirection( glm::vec3 direction ) const{
		glm::vec3 maxPoint = glm::vec3(0);
		float   maxDistance = -FLT_MAX;

		std::vector<glm::vec3> vertices = GetVertices();

		for ( glm::vec3 vertex : vertices ) {
			float distance = glm::dot( vertex, direction );
			if ( distance > maxDistance ) {
				maxDistance = distance;
				maxPoint = vertex;
			}
		}
		return maxPoint;
	}
};

class OBB : public Collider{
public:
	glm::vec3	center;
	glm::mat3	u; //rotation matrix
	glm::vec3	e; // pos halfwidth extents of oobb
public:
	glm::vec3 GetMin() const{
		return ( e * u) ;
	}

	glm::vec3 GetMax() const{
		return ( e * u) ;
	}
	
	glm::vec3 GetForward() {
		return glm::vec3( 0, 0, 1 ) * u;
	}

	glm::vec3 GetRight() {
		return glm::vec3( 1, 0, 0 ) * u;
	}

	glm::vec3 GetUp() {
		return glm::vec3( 0, 1, 0 ) * u;
	}

	virtual std::vector<glm::vec3> GetVertices() const {
		std::vector<glm::vec3> verts;
		//TODO GET VERTICES
		assert( 0 );
		return verts;
	}
};

class AABB : public Collider {
public:
	glm::vec3 center;
	glm::vec3 e;

	inline glm::vec3 GetMin() const{
		return center - e;
	}

	inline glm::vec3 GetMax() const{
		return center + e;
	}

	virtual std::vector<glm::vec3> GetVertices() const {
		std::vector<glm::vec3> vertices( 8 );
		glm::vec3 min = GetMin();
		glm::vec3 max = GetMax();
		
		vertices[0] = min;
		vertices[1] = glm::vec3( min.x, min.y, max.z );
		vertices[2] = glm::vec3( min.x, max.y, min.z );
		vertices[3] = glm::vec3( max.x, min.y, min.z );
		vertices[4] = glm::vec3( min.x, max.y, max.z );
		vertices[5] = glm::vec3( max.x, min.y, max.z );
		vertices[6] = glm::vec3( max.x, max.y, min.z );
		vertices[7] = max;
		return vertices;
	}
	

};	
class ConvexCollider : public Collider {
public:
	//this caches vertices since you cant calc
	std::vector<glm::vec3> vertices;

	virtual std::vector<glm::vec3> GetVertices() const {
		return vertices;
	}
};
