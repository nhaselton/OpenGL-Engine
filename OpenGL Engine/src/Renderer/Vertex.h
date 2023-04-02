#pragma once
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 pos;  
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 biTangent;
	glm::vec4 weights;
	glm::ivec4 boneIDs;
};