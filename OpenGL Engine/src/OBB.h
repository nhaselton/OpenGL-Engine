#pragma once
#include "glm/glm.hpp"

class Plane {
public:
	glm::vec3	n;
	float		d;
};

class OBB {
public:
	glm::vec3	center;
	glm::mat3	u; //rotation matrix
	glm::vec3	e; // pos halfwidth extents of oobb
public:
	glm::vec3 GetMin() const{
		return ( center - e ) * u;
	}

	glm::vec3 GetMax() const{
		return ( center + e ) * u;
	}
	
	glm::vec3* GetVertices() {
		glm::vec3 min = GetMin();
		glm::vec3 max = GetMax();

		glm::vec3 vertices[8] = {
		glm::vec3( min.x, min.y, min.z ),
		glm::vec3( min.x, min.y, max.z ),
		glm::vec3( min.x, max.y, min.z ),
		glm::vec3( min.x, max.y, max.z ),
		glm::vec3( max.x, min.y, min.z ),
		glm::vec3( min.x, min.y, max.z ),
		glm::vec3( max.x, max.y, min.z ),
		glm::vec3( max.x, max.y, max.z )
		};
		return vertices;
	}

	glm::vec3* GetAxes() {
		glm::vec3 angle = glm::eulerAngles( glm::quat( u ) );
		
		glm::vec3 forward;
		forward.x = cos( angle.x ) * sin( angle.y );
		forward.y = -sin( angle.x );
		forward.z = cos( angle.x ) * cos( angle.y );

		glm::vec3 right;
		right.x = cos( angle.y );
		right.y = 0;
		right.z = -sin( angle.y );

		glm::vec3 up = glm::cross( forward, right );
	}
};