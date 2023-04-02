#include "ResourceManager.h"
#include "../Tools/Tools.h"
#include <string>
#include <iostream>
ResourceManager ResourceManager::instance;

ResourceManager::ResourceManager() {
}

Model* ResourceManager::GetModel( const char* path ) {
	if ( modelMap.find( path ) != modelMap.end() )
		return &modelMap[path];
	else {
		modelMap[path] = LoadStaticModelGLTF( path );
		return &modelMap[path];
	}
}

Animation* ResourceManager::GetAnimation( std::string path ) {
	if ( animationMap.find( path ) != animationMap.end() )
		return &animationMap[path];

	std::cout << "animation " << path << " not found" << std::endl;
	return nullptr;
}

Texture* ResourceManager::GetTexture( std::string path) {
	if ( textureMap.find( path ) != textureMap.end() )
		return &textureMap[path];
	else {
		textureMap[path] = LoadTexture(path.c_str(),TEXTURE_TYPE_ALBEDO);
		return &textureMap[path];
	}


}