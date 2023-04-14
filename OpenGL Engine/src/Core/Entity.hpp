#pragma once
#include <glm/glm.hpp>
#include "../Physics/RigidBody.h"
#include "../Renderer/Model.h"
#include "../Renderer/EntModel.h"
#include "Transform.h"
#include "../Physics/Colliders.h"

class Entity {

public:
	Entity();
	Entity(glm::vec3 pos);
	Entity(Transform t);
	
	//priviteize 
	Transform	transform;
	EntModel	model;
	OBB			boundingBox;
	RigidBody	rigidBody;
};