#include "Colliders.h"
#include "../Framework/ResourceManager.h"
#include "../Renderer/Mesh.h"
#include "../Tools/Tools.h"

//probably can remove but keeping for now 
Hull* CreateBoxHull( glm::vec3 offset, glm::vec3 scale ) {
	HullInfo h = LoadHullGLTF( "res/models/gltf/hulls/cube.gltf" ); //todo make this return hull
	Hull* hull = new Hull( h.vertexPositions, h.indices );
	return hull;
};

Hull::Hull( std::vector<glm::vec3> vertices, std::vector<unsigned short> indices ) {
	this->vertices = vertices;
	this->indices = indices;
	this->drawMesh = new Mesh( this->vertices , this->indices);
	c = glm::vec3( 0 ); 
	colliderType = COLLIDER_HULL;
}