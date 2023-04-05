#pragma once
#include <glm/glm.hpp>

class Transform {
public:
	Transform();
	Transform(glm::vec3 pos);
	Transform(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale);

	glm::vec3			Position() const { return position; }
	glm::vec3			Rotation() const { return rotation; }
	glm::vec3			Scale() const { return scale; }
	
	void				SetPosition( glm::vec3 p ) { position = p; }
	void				SetRotation( glm::vec3 r ) { rotation = r; }
	void				SetScale( glm::vec3 s ) { scale = s; }
	
	void				Rotate( glm::vec3 rotation ) { this->rotation += rotation; };
	void				Translate( glm::vec3 translate ) { this->position += translate; };
	void				ScaleTransform( glm::vec3 scale ) { this->scale += scale; }

	void				Update() { position += velocity; rotation += rotationalVelocity; }
	glm::mat4			Matrix();
//private:
public:
	glm::vec3			position;
	glm::vec3			rotation;
	glm::vec3			scale;
	glm::vec3			velocity;
	glm::vec3			rotationalVelocity;
};