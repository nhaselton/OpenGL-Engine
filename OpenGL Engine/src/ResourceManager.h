#pragma once
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "Animation.h"
#include <string>
#include <map>

class ResourceManager {
public:
	Model* GetModel(const char* path);
	Texture* GetTexture(std::string path);
	Animation* GetAnimation( std::string path );
	static ResourceManager& Get() { return instance; }

private:
	ResourceManager();
	static ResourceManager instance;

	std::map<const char*, Mesh> meshMap;  //todo change to std::String
	std::map<const char*, Model> modelMap;//todo change to std::String
	std::map<std::string, Texture> textureMap;
	std::map<std::string, Animation> animationMap;
};