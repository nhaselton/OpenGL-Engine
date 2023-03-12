#pragma once
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include <string>
#include <map>

class ResourceManager {
public:
	Model* GetModel(const char* path);
	Texture* GetTexture(std::string path);
	static ResourceManager& Get() { return instance; }

private:
	ResourceManager();
	static ResourceManager instance;

	std::map<const char*, Mesh> meshMap;
	std::map<const char*, Model> modelMap;
	std::map<std::string, Texture> textureMap;
};