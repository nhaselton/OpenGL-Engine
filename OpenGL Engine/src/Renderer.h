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

struct SkyBox {
	unsigned int cubeMapTex;
	unsigned int vbo;
	unsigned int vao;
};

#define SHADOW_ATLAS_WIDTH 8192
#define SHADOW_ATLAS_HEIGHT 8192
#define SHADOW_ATLAS_TILE_SIZE 512 //tile size


#define bitset(byte,nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte,nbit) ((byte) &= ~(1<<(nbit)))
#define bitflip(byte,nbit)  ((byte) ^=  (1<<(nbit)))
#define bitcheck(byte,nbit) ((byte) &   (1<<(nbit)))

class Shader;

class Renderer {
public:
	Renderer();
	void			Init(class Window* window , Camera* camera);
	void			BeginFrame();
	void			DrawFrame(std::vector<Entity>& meshes, std::vector<Light>& lights, double interp );
	void			EndFrame();

private:
	Shader*			staticShader;
	Shader*			dynamicShader;
	Shader*			staticShadowCubeMapAtlasShader;
	Shader*			dynamicShadowCubeMapAtlasShader;
	Shader*			staticDepthPrepassShader;
	Shader*			staticShadowShader;
	Shader*			dynamicShadowShader;
	Shader*			debugDepthQuadShader;
	Shader*			skyboxShader;	

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

	//debug quad
	unsigned int	quadVAO = 0;
	unsigned int	quadVBO;

	//cubemap
	Texture			cubeMap;

	bool			showNormalMap;
	bool			showSpecularMap;
	bool			showShadowAtlas;

	// (8192 * 8192) / (512 * 512) bit representation of shadowAltas
	unsigned int*	shadowAtlasContents;
	SkyBox			skybox;


private:
	void			DrawModelR( Shader* shader, Model* model, Node* root , bool shouldTexture, glm::mat4 parent = glm::mat4(1.0));
	void			DrawPointLight( Light& light , std::vector<Entity>& entities );
	void			DrawSpotLight( Light& light, std::vector<Entity>& entities );
	void			DrawDirectionalLight( Light& light , std::vector<Entity>& entities );
	void			InitLights( std::vector<Light> lights );
	void			BindTextures( Mesh* mesh );
	void			renderQuad();
	void			CreateShadowAtlas();
	void			DebugPrintShadowAtlas();
	//returns UV coords of texture atlas slot
	AtlasLocation	FindFreeSpaceInShadowAltas( shadowMapType type, int shadowWidth, int shadowHeight );
	void			ComputeHierarchy( Animation* animation, float time, Node* node, glm::mat4 parent = glm::mat4( 1.0 ) );
	void			DrawScene( Shader* staticShader, Shader* dynamicShader, bool drawTextures, std::vector<Entity>& entities );
	SkyBox			CreateSkyBox(const char* paths[6]);
	void			DrawSkyBox();
	};
