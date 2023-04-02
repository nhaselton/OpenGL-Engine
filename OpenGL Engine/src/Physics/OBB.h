#pragma once
#include <iostream>

#include "glm/gtx/string_cast.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

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
		return ( e ) ;
	}

	glm::vec3 GetMax() const{
		return ( e ) ;
	}


	
	glm::vec3* GetAxes() {
	
		glm::vec3 axes[3]{
			glm::vec3( 0,0,1 ) * u,
			glm::vec3( 1,0,0 ) * u, 
			glm::vec3( 0,1,0 ) * u,
		};


		return axes;
	}
};
