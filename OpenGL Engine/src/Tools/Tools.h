#pragma once
#include "../renderer/Mesh.h"
#include "../renderer/Model.h"
#include "../renderer/Texture.h"

struct HullInfo {
	std::vector<glm::vec3> vertexPositions;
	std::vector<unsigned short> indices;
};

Model			LoadStaticModelGLTF( const char* path );
void			LoadAnimations( const char* path );
Texture			LoadTexture( const char* path, TextureType textureType );
Texture			LoadCubeMapTexture(const char* path[6]);
//Just gives positions and indices in local space	( no scale applied )
HullInfo		LoadHullGLTF(const char* path);