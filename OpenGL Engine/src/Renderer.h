#pragma once
#include "Entity.hpp"
#include <vector>
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"

struct DeferredPath {
	unsigned int gFrameBuffer;
	
	unsigned int positionTexture;
	unsigned int normalTexture;
	unsigned int albedoTexture;
};

class Renderer {
public:
	Renderer();
	void			Init(class Window* window , Camera* camera);
	void			BeginFrame();
	void			DrawFrame(std::vector<Entity>& meshes, std::vector<Light>& lights);
	void			EndFrame();

private:
	class Shader*	shader;
	class Shader*	lightShader;

	class Shader*	geoPassShader;
	class Shader*	screenShader;

	class Window*	window;
	Camera*			camera;
	
	glm::mat4		view;
	glm::mat4		projection;

	DeferredPath	deferredPath;

	bool			showNormalMap;
	bool			showSpecularMap;

private:
	void DrawModelR(Model* model, Node* root , glm::mat4 parent = glm::mat4(1.0));

};