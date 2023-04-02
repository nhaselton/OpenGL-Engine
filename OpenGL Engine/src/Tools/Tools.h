#pragma once
#include "../renderer/Mesh.h"
#include "../renderer/Model.h"
#include "../renderer/Texture.h"

Model			LoadStaticModelGLTF( const char* path );
void			LoadAnimations( const char* path );
Texture			LoadTexture( const char* path, TextureType textureType );
Texture			LoadCubeMapTexture(const char* path[6]);