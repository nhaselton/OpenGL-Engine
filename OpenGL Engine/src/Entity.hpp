#pragma once
#include "Model.h"
#include "Transform.h"
#include <glm/glm.hpp>
class Entity {

public:
	Entity();
	Entity(glm::vec3 pos);
	Entity(Transform t);
	
	//priviteize 
	Transform transform;
	Model* model;
};