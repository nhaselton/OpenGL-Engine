#pragma once
#include "Model.h"
#include "Animation.h"

#include <string>
#include <vector>

//Ent model holds renderModel's anim info, node data
struct NodeDataStruct {
		std::string name;
		glm::mat4 position;
		glm::mat4 boneData;
};

class EntModel{
public:
	Animator				animator;

public:
	void					SetRenderModel( Model* model );
	void					CalculateNodesR( Node* node , glm::mat4 parent );
	Model*					GetRenderModel() { return renderModel; }

	std::vector<NodeDataStruct> nodeData;

private:
	Model*					renderModel;
};