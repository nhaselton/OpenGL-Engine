#include "Entity.hpp"

Entity::Entity() {
	transform = Transform();
	model.SetRenderModel( nullptr );
}

Entity::Entity( glm::vec3 pos ) {
	transform.SetPosition( pos );
	model.SetRenderModel( nullptr );
}

Entity::Entity( Transform t ) {
	transform = t;
	model.SetRenderModel( nullptr );
}