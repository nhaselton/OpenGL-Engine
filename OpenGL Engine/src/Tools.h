#pragma once
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"

Model LoadStaticModelGLTF( const char* path );
Texture LoadTexture( const char* path ,TextureType textureType);