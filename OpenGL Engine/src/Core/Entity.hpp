#pragma once
#include <glm/glm.hpp>
#include "../Renderer/Model.h"
#include "../Renderer/EntModel.h"
#include "Transform.h"
#include "../Physics/OBB.h"
#include "../Physics/RigidBody.h"
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