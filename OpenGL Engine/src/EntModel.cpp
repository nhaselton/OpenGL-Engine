#include "EntModel.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
void EntModel::SetRenderModel(Model* model) {
	renderModel = model;
	
	//make sure there are enough spots for nodes
	if ( renderModel ) {
		if ( nodeData.size() < model->nodes.size() )
			nodeData.resize( model->nodes.size() );
	}
}

void EntModel::CalculateNodesR( Node* node , glm::mat4 parent ) {
	glm::mat4 nodeTransformed = animator.ComputeNode( node );
	glm::mat4 here = parent * nodeTransformed;
	
	nodeTransformed = node->t.Matrix();
	nodeData[node->index].position = here;//nodeTransformed;
	nodeData[node->index].name = node->name;
	nodeData[node->index].boneData = here * node->inverseBind;

	for ( int i = 0; i < node->children.size(); i++ )
		CalculateNodesR( node->children[i], here );
}
