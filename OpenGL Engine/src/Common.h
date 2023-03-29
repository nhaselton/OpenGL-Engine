#pragma once
#include "Mesh.h"
#include "Entity.hpp"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
class Common {
public:
	Common();
	void				Init();
	void				Frame();
	void				UpdateInput();

	void				InitGraphicsScene();
	void				InitPhysicsScene();
private:
	Camera				camera;

	class Window*		window;
	class Renderer*		renderer;

	std::vector<Entity>	entites;
	std::vector<Light>	lights;

	unsigned int		width;
	unsigned int		height;

	//Timing for tik system
	double				lastTime;
	double				accum;
	double				tickRate;
};