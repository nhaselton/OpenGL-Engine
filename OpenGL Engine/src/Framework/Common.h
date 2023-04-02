#pragma once
#include "../Renderer/Mesh.h"
#include "../core/Entity.hpp"
#include "../core/Camera.h"
#include "../renderer/Texture.h"
#include "../renderer/Light.h"
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