#pragma once

class BoxCollider {
public:
	BoxCollider() {
		c = glm::vec3( 0 );
		e = glm::vec3(1.0f);
		u = glm::mat3( 1.0 );
	}
	
	BoxCollider( glm::vec3 c, glm::vec3 hl ) {
		this->c = c;
		this->e = hl;
		u = glm::mat3( 1.0 );
	}

	glm::vec3 c; // local space (apply entity transformation) 
	glm::vec3 e; // halfwidth
	glm::mat3 u;
};