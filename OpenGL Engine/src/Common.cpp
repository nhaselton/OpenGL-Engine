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
	//camera.transform.SetPosition( glm::vec3( -1.6, 2.2, -0.1 ) );
	//camera.transform.SetRotation( glm::vec3( 0.0, 3.1, 0.0) );
	camera.transform.SetPosition( glm::vec3( 0, 1, -4.5 ) );
	camera.transform.SetRotation( glm::vec3( 0, 1.5, 0 ) );

	window = new Window;
	window->Init();

	renderer = new Renderer;
	renderer->Init( window , &camera);

	//Entity e2;
	//e2.model = ResourceManager::Get().GetModel( "res/models/gltf/SPONZA/SPONZA.gltf" );
	////e2.model = ResourceManager::Get().GetModel( "res/models/gltf/cube.glb.gltf" );
	//e2.transform.SetPosition( glm::vec3( 0, 0, 0 ) );
	//entites.push_back( e2 );


	Entity e3;
	//e3.model = ResourceManager::Get().GetModel( "res/models/gltf/simple_skin.gltf" );
	e3.model = ResourceManager::Get().GetModel( "res/models/gltf/ron.gltf" );
	//e3.model = ResourceManager::Get().GetModel( "res/models/gltf/simple_skin_2.gltf" );
	//e3.model = ResourceManager::Get().GetModel( "res/models/brain/BrainStem.gltf" );
	
	entites.push_back( e3 );

	Light pointLight = {};
	pointLight.lType = LIGHT_POINT;
	pointLight.color = glm::vec3( 1, 1, 1 );
	pointLight.cutoff = glm::cos( glm::radians( 12.0f ) );
	pointLight.direction = camera.GetForward();
	pointLight.linear = 0.022;
	pointLight.outerCutoff = glm::cos( glm::radians( 17.5f ) );
	pointLight.pos = glm::vec3( -4.9, 1.2f, 0.0f );
	pointLight.quadratic = 0.019;
	lights.push_back( pointLight );

	Light directional;
	directional.lType = LIGHT_DIRECTIONAL;
	directional.color = glm::vec3( 1 );
	directional.direction = glm::vec3(.25f,.35f,0.0f);
	lights.push_back( directional );

	Light spot;
	spot.lType = LIGHT_SPOT;
	spot.color = glm::vec3( 1, 1, 1 );
	spot.cutoff = glm::cos( glm::radians( 12.0f ) );
	spot.direction = glm::vec3(0,-.1,-1);
	spot.linear = 0.022;
	spot.outerCutoff = glm::cos( glm::radians( 17.5f ) );
	spot.pos = glm::vec3( -.5, 1.5, 3.7);
	spot.quadratic = 0.019;
	lights.push_back( spot );

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
		camera.transform.Translate( camera.GetForward()  * .001f);
	}
	
	if ( Input::keys[GLFW_KEY_S] ) {
		camera.transform.Translate( -camera.GetForward() * .001f );
	}
	
	if ( Input::keys[GLFW_KEY_A] ) {
		camera.transform.Translate( -glm::cross(camera.GetForward(), glm::vec3(0,1,0)) * .001f );
	}
	
	if ( Input::keys[GLFW_KEY_D] ) {
		camera.transform.Translate( glm::cross( camera.GetForward(), glm::vec3( 0, 1, 0 ) ) * .001f );
	}

	if ( Input::keys[GLFW_KEY_LEFT] ) {
		camera.transform.Rotate(glm::vec3(0, -.001f,0.0f));
	}

	if ( Input::keys[GLFW_KEY_RIGHT] ) {
		camera.transform.Rotate( glm::vec3( 0, .001f, 0.0f ) );
	}

	if ( Input::keys[GLFW_KEY_DOWN] ) {
		camera.transform.Rotate( glm::vec3(  -.001f, 0, 0.0f ) );
	}

	if ( Input::keys[GLFW_KEY_UP] ) {
		camera.transform.Rotate( glm::vec3(  .001f, 0, 0.0f ) );
	}



}