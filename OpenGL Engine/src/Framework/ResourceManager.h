#pragma once
#include "../renderer/Mesh.h"
#include "../renderer/Model.h"
#include "../renderer/Texture.h"
#include "../renderer/Animation.h"
#include <string>
#include <map>
#include <iostream>

class ResourceManager {
public:
	Model* GetModel(const char* path);
	Texture* GetTexture(std::string path);
	Animation* GetAnimation( std::string path );
	static ResourceManager& Get() { return instance; }
	bool ContainsAnimation( std::string path ) { return ( animationMap.find( path ) != animationMap.end() ); }
	void CreateAnimation( std::string path ) { Animation a; animationMap[path.c_str()] = a; std::cout << path << std::endl; }//create animation and do nothing 
private:
	ResourceManager();
	static ResourceManager instance;

	std::map<std::string, Mesh> meshMap;  
	std::map<std::string, Model> modelMap;
	std::map<std::string, Texture> textureMap;
	std::map<std::string, Animation> animationMap;
};