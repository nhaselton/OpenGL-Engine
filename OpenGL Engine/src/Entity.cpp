#include "Entity.hpp"

Entity::Entity() {
	transform = Transform();
	model = nullptr;
}

Entity::Entity( glm::vec3 pos ) {
	transform.SetPosition( pos );
	model = nullptr;
}

Entity::Entity( Transform t ) {
	transform = t;
	model = nullptr;
}