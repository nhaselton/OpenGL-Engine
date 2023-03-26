#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Common.h"
#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Input.h"
#include "Tools.h"
#include "ResourceManager.h"

#include <glm/gtx/string_cast.hpp>

Common::Common() {
}

void Common::Init() {
	float vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
	   -0.5f, -0.5f, 0.0f,  // bottom left
	   -0.5f,  0.5f, 0.0f   // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	camera.model = nullptr;


	window = new Window;
	window->Init();

	renderer = new Renderer;
	renderer->Init( window , &camera);

	Entity e2;
	e2.model = ResourceManager::Get().GetModel( "res/models/gltf/sponza/sponza.gltf" );
	e2.model->isStatic = true;
	entites.push_back( e2 );

	Entity e3;
	e3.model = ResourceManager::Get().GetModel( "res/models/gltf/imp/imp.gltf" );
	e3.model->isStatic = false;
	//LoadAnimations("res/models/gltf/imp/anim/imp_run_forward.gltf");
	entites.push_back( e3 );

	Light pointLight = {};
	pointLight.lType = LIGHT_POINT;
	pointLight.color = glm::vec3( 1, 1, 1 );
	pointLight.direction = camera.GetForward();
	pointLight.linear = 0.022;
	pointLight.pos = glm::vec3( 1.4, 2, -.1f );
	pointLight.quadratic = 0.019;
	pointLight.shadowAtlasLocation.index = glm::ivec2( -1, -1 );
	pointLight.farPlane = 25.0f;
	pointLight.shadowMapSize = glm::vec2( 1024 );
	pointLight.hasShadow = true;
	lights.push_back( pointLight );
	
	Light directional {};
	directional.lType = LIGHT_DIRECTIONAL;
	directional.color = glm::vec3( 1 );
	directional.pos = glm::vec3( -8.3,14.2,-0.8);
	directional.direction = glm::vec3( -0.7,-0.7,-0.1);
	directional.shadowAtlasLocation.index = glm::ivec2( -1, -1 );
	directional.farPlane = 25.0f;
	directional.shadowMapSize = glm::vec2( 1024 );
	directional.hasShadow = true;
	lights.push_back( directional );

	Light spot;
	spot.lType = LIGHT_SPOT;	
	spot.color = glm::vec3( 1, 1, 1 );
	spot.cutoff = glm::cos( glm::radians( 30.0f ) );
	spot.linear = 0.022;
	spot.outerCutoff = glm::cos( glm::radians( 45.f ) );
	spot.pos = glm::vec3( -1.4, 1, 0.0);
	spot.direction = glm::vec3(11,0.1,-0.8);
	spot.quadratic = 0.019;	
	spot.farPlane = 25.0f;
	spot.shadowAtlasLocation.index = glm::ivec2( -1, -1 );
	spot.shadowMapSize = glm::vec2(1024);
	spot.hasShadow = true;
	lights.push_back( spot );
	
	camera.transform.SetRotation(glm::vec3( 0, 1, 0 ));
}

void Common::Frame() {
	while ( !glfwWindowShouldClose( window->GetHandle() ) ) {
		glfwPollEvents();
		UpdateInput();

		renderer->BeginFrame();
		renderer->DrawFrame( entites , lights);
		renderer->EndFrame();
	}
}

//TODO
void Common::UpdateInput() {
	if ( Input::keys[GLFW_KEY_ESCAPE] ) {
		exit( 0 );
	}

	if ( Input::keys[GLFW_KEY_W] ) {
		camera.transform.Translate( camera.GetForward()  * .01f);
	}
	
	if ( Input::keys[GLFW_KEY_S] ) {
		camera.transform.Translate( -camera.GetForward() * .01f );
	}
	
	if ( Input::keys[GLFW_KEY_A] ) {
		camera.transform.Translate( -glm::cross(camera.GetForward(), glm::vec3(0,1,0)) * .01f );
	}
	
	if ( Input::keys[GLFW_KEY_D] ) {
		camera.transform.Translate( glm::cross( camera.GetForward(), glm::vec3( 0, 1, 0 ) ) * .01f );
	}

	if ( Input::keys[GLFW_KEY_LEFT] ) {
		camera.transform.Rotate(glm::vec3(0, -.005f,0.0f));
	}

	if ( Input::keys[GLFW_KEY_RIGHT] ) {
		camera.transform.Rotate( glm::vec3( 0, .005f, 0.0f ) );
	}

	if ( Input::keys[GLFW_KEY_DOWN] ) {
		camera.transform.Rotate( glm::vec3(  -.005f, 0, 0.0f ) );
	}

	if ( Input::keys[GLFW_KEY_UP] ) {
		camera.transform.Rotate( glm::vec3(  .005f, 0, 0.0f ) );
	}



}