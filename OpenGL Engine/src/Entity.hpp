#pragma once
#include <glm/glm.hpp>
#include "Model.h"
#include "EntModel.h"
#include "Transform.h"
#include "OBB.h" 
class Entity {

public:
	Entity();
	Entity(glm::vec3 pos);
	Entity(Transform t);
	
	//priviteize 
	Transform	transform;
	EntModel	model;
	OBB			boundingBox;
};