#pragma once
#include <glm/glm.hpp>

struct AtlasLocation {
	glm::ivec2 index;
	glm::vec4 texCoords;
};

enum LightType {
	LIGHT_POINT = 0,
	LIGHT_DIRECTIONAL = 1,
	LIGHT_SPOT = 2
};

struct Light {
	LightType lType;
	//All
	glm::vec3 color;
	//Directional
	glm::vec3 direction;
	//point
	glm::vec3 pos;
	float linear;
	float quadratic;
	//cutoff
	float cutoff;//12.5f
	float outerCutoff;//17.5f

	//Point light shadows
	float farPlane;

	//shadow matrix
	bool hasShadow;
	glm::mat4 lightSpaceMatrix;

	//shadow atlas
	//glm::vec4 shadowUVs;//0,0
	AtlasLocation shadowAtlasLocation;
};