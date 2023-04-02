#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Mesh.h"
#include "../Core/Transform.h"
#include "Animation.h"

struct Node {
	Node(){}
	std::string			name;
	int					index;
	Transform			t;
	std::vector<unsigned int> meshIndices;;
	std::vector<Node*>	children;
	int					meshIndex;

	int					boneID;//fucking kill me 

	bool				isJoint;
	glm::mat4			inverseBind;

	//compute before renderering and render stuff with this
	glm::mat4			computedOffset;
};

struct Skin {
	Skin() {}
	std::string name;
	std::vector<int> jointIDs;
	std::vector<glm::mat4> inverseBindMatrices;
};

class Model {
public:
	Model() {}
	bool					isStatic;
	std::vector<Node>		nodes;
	std::vector<Mesh>		meshes;
	Skin*					skin;
	int						rootNode;

};