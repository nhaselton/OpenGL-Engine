#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Mesh.h"
#include "Transform.h"
#include <string>
#include <vector>

struct TranslationKeyFrame {
	glm::vec3 translation;
	float time;
};

struct RotationKeyFrame {
	glm::quat rotaiton;
	float time;
};

struct ScaleKeyFrame {
	glm::vec3 scale;
	float time;
};


struct AnimChannel{
	int nodeID;	

	std::vector<TranslationKeyFrame> translations;
	std::vector<RotationKeyFrame> rotations;
	std::vector<ScaleKeyFrame> scales;
};

struct Animation {
	std::string name;
	std::vector<AnimChannel> animChannels;
};


struct Node {
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