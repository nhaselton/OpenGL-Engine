#pragma once
#include "Entity.hpp"
#include <vector>
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"

enum shadowMapType {
	SHADOW_MAP_OMNIDIRECTIONAL,
	SHADOW_MAP_CUBE
};

#define VIEWPORT_INDEX_SIZE 512 //how much to multiply the viewport by for shadow atlas

class Renderer {
public:
	Renderer();
	void			Init(class Window* window , Camera* camera);
	void			BeginFrame();
	void			DrawFrame(std::vector<Entity>& meshes, std::vector<Light>& lights);
	void			EndFrame();

private:
	class Shader*	staticShader;
	class Shader*	dynamicShader;
	class Shader*	staticShadowCubeMapAtlasShader;
	class Shader*	staticShadowShader;
	class Shader*	debugDepthQuadShader;


	class Window*	window;
	Camera*			camera;
	
	glm::mat4		view;
	glm::mat4		projection;

	//BufferList		buffers;
	unsigned int	shadowMapFBO;
	unsigned int	depthMapImage;
	
	unsigned int	cubeShadowMapFBO;
	unsigned int	depthCubeMap;

	unsigned int	shadowAtlasFBO;
	unsigned int	shadowAtlasImage;


	bool			showNormalMap;
	bool			showSpecularMap;
	bool			showShadowAtlas;

	// (8192 * 8192) / (512 * 512) bit representation of shadowAltas
	unsigned int*	shadowAtlasContents;

private:
	void			DrawModelR( Shader* shader, Model* model, Node* root , bool shouldTexture, glm::mat4 parent = glm::mat4(1.0));
	void			DrawPointLight( Light& light , std::vector<Entity>& entities );
	void			DrawSpotLight( Light& light , std::vector<Entity>& entities );
	void			DrawDirectionalLight( Light& light , std::vector<Entity>& entities );
	void			InitLights( std::vector<Light> lights );
	void			BindTextures( Mesh* mesh );
	void			CreateDepthMap();
	void			CreateCubeMap();
	void			renderQuad();
	void			CreateShadowAtlas();
	void			DebugPrintShadowAtlas();
	//returns UV coords of texture atlas slot
	AtlasLocation		FindFreeSpaceInShadowAltas( shadowMapType type, int shadowWidth, int shadowHeight );
};