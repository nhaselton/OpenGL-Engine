#include <vector>
#include <iostream>
#include <chrono>

#include <GLFW/glfw3.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ResourceManager.h"
#include "Common.h"
#include "Window.h"
#include "Input.h"

#include "../renderer/Renderer.h"
#include "../renderer/Mesh.h"

#include "../tools/Tools.h"
#include "../physics/Colliders.h"
#include "../physics/Collisions.h"
#include "../Physics/Physics.h"
Common::Common() {
	width = 1280;
	height = 720;
}

void Common::Init() {
	window = new Window;
	window->Init( width, height );

	renderer = new Renderer;
	renderer->Init( window, &camera );

#if 0
	InitGraphicsScene();
#else
	InitPhysicsScene();
#endif
	lastTime = glfwGetTime();
	tickRate = 1.0 / 60.0;
	accum = 0.0;
}


void Common::Frame() {

	while ( !glfwWindowShouldClose( window->GetHandle() ) ) {

		double now = glfwGetTime();
		accum += ( now - lastTime );



		while ( accum > tickRate ) {
			glfwPollEvents();
			UpdateInput();


			accum -= tickRate;

	//  AABB Continious
#if 0 
			for ( int i = 0; i < entites.size(); i++ ) {

				AABB aabbA;
				aabbA.center = entites[0].boundingBox.center;
				aabbA.e = entites[0].boundingBox.e;

				AABB aabbB;
				aabbB.center = entites[1].boundingBox.center;
				aabbB.e = entites[1].boundingBox.e;

				//Collision Testing


				if ( entites[i].rigidBody.velocity != glm::vec3(0) ) {

					ContinuousHitInfo hi = TestContinuousAABB( aabbA, aabbB, entites[0].rigidBody.velocity, entites[1].rigidBody.velocity );
					if ( !hi.hit ) {
						std::cout << "HIT\n";

						entites[i].transform.position += entites[i].rigidBody.velocity;
						entites[i].boundingBox.center = entites[i].transform.Position();
					}
					//if hit, calc time and move by that much
					else {
						glm::vec3 newVel = hi.time * entites[i].rigidBody.velocity;
						entites[i].transform.position += newVel;
						entites[i].boundingBox.center = entites[i].transform.Position();
						entites[i].rigidBody.velocity = glm::vec3( 0 );
					}
				}
			}
#endif 

			AABB aabbA;
			aabbA.center = entites[0].boundingBox.center;
			aabbA.e = entites[0].boundingBox.e;

			AABB aabbB;
			aabbB.center = entites[1].boundingBox.center;
			aabbB.e = entites[1].boundingBox.e;

			entites[0].boundingBox.center = entites[0].transform.position;
			entites[1].boundingBox.center = entites[1].transform.position;
			
			HitInfo he = GJK( aabbA, aabbB );
			if ( he.hit )
				std::cout << glm::to_string(he.normal) << ", " << he.depth << std::endl;


		}
		
		if ( Input::keys[GLFW_KEY_P] )
			entites[0].boundingBox.u = glm::mat3( 1 );

		glm::vec3 p0 = camera.transform.Position();
		glm::vec3 p1 = p0 + ( camera.GetForward() * 100.0f );


		float interp = ( float ) ( accum / tickRate );
		renderer->BeginFrame();
		renderer->DrawFrame( entites, lights, interp );
		renderer->EndFrame();

		lastTime = now;
	}
}


void Common::InitPhysicsScene() {
	glm::vec3 box1Pos = glm::vec3( 0 );
	glm::vec3 box2Pos = glm::vec3( 2.5, 0, 0 );

	glm::mat3 box1Rot( 1.0 );
	float t = glm::radians( 45.0f );
#if 0
	box1Rot[0] = glm::vec3( -sinf( t ), cosf( t ), 0 );
	box1Rot[1] = glm::vec3( cos( t ), sinf( t ), 0 );
	box1Rot[2] = glm::vec3( 0, 0, 1 );
#endif


	camera.transform.SetPosition( glm::vec3( 0, 0, -5 ) );
	camera.transform.SetRotation( glm::vec3( 0, glm::radians( 90.f ), 0 ) );

	//Box
	Entity box;
	box.model.SetRenderModel( ResourceManager::Get().GetModel( "res/models/gltf/cube.gltf" ) );
	box.transform.SetPosition( box1Pos );

	OBB obb;
	obb.center = glm::vec3( box.transform.Position() );
	obb.u = box1Rot;
	obb.e = glm::vec3( 1.0f );
	box.boundingBox = obb;
	obb.type = COLLIDER_OBB;
	//box.rigidBody.AddForce( glm::normalize( glm::vec3( 1, 1, 0 )  ) * .15f );
	entites.push_back( box );

	//Monkey
	Entity monkey;
	monkey.model.SetRenderModel( ResourceManager::Get().GetModel( "res/models/gltf/monkey.gltf" ) );
	monkey.transform.SetPosition( box2Pos );

	OBB obb2;
	obb2.center = glm::vec3( monkey.transform.Position() );
	obb2.u = glm::mat3( 1.0 );
	obb2.e = glm::vec3( 1.0f );
	obb2.type = COLLIDER_OBB;
	monkey.boundingBox = obb2;
	entites.push_back( monkey );

	entites[0].rigidBody.velocity = glm::vec3( .2f, 0, 0 );
	entites[1].rigidBody.velocity = glm::vec3( 0, 0, 0 );
}

void Common::UpdateInput() {
	if ( Input::keys[GLFW_KEY_ESCAPE] ) {
		exit( 0 );
	}

	camera.rigidBody.velocity = glm::vec3( 0 );
	camera.rigidBody.angularVelocity = glm::vec3( 0 );

	if ( Input::keys[GLFW_KEY_W] ) {
		camera.rigidBody.velocity += ( camera.GetForward() * glm::vec3( .1f ) );
	}

	if ( Input::keys[GLFW_KEY_S] ) {
		camera.rigidBody.velocity += ( -camera.GetForward() * glm::vec3( .1f ) );
	}

	if ( Input::keys[GLFW_KEY_A] ) {
		camera.rigidBody.velocity += ( -glm::cross( camera.GetForward(), glm::vec3( 0, 1, 0 ) ) * glm::vec3( .1f ) );
	}
	if ( Input::keys[GLFW_KEY_D] ) {
		camera.rigidBody.velocity += ( glm::cross( camera.GetForward(), glm::vec3( 0, 1, 0 ) ) * glm::vec3( .1f ) );
	}

	if ( Input::keys[GLFW_KEY_LEFT] ) {
		camera.rigidBody.angularVelocity += ( glm::vec3( 0, -.5f, 0.0f ) * glm::vec3( .1f ) );
	}

	if ( Input::keys[GLFW_KEY_RIGHT] ) {
		camera.rigidBody.angularVelocity += ( glm::vec3( 0, .5f, 0.0f ) * glm::vec3( .1f ) );
	}

	if ( Input::keys[GLFW_KEY_DOWN] ) {
		camera.rigidBody.angularVelocity += ( glm::vec3( -.5f, 0, 0.0f ) * glm::vec3( .1f ) );
	}

	if ( Input::keys[GLFW_KEY_UP] ) {
		camera.rigidBody.angularVelocity += ( glm::vec3( .5f, 0, 0.0f ) * glm::vec3( .1f ) );
	}

	camera.transform.position += camera.rigidBody.velocity;
	camera.transform.rotation += camera.rigidBody.angularVelocity;
}


void Common::InitGraphicsScene() {
	camera.transform.SetPosition( glm::vec3( -.6, 1, -4 ) );
	camera.transform.SetRotation( glm::vec3( .001, -0.1, 1.0 ) );

	Entity e3;
	e3.model.SetRenderModel( ResourceManager::Get().GetModel( "res/models/gltf/imp/imp.gltf" ) );
	//e3.model.SetRenderModel( ResourceManager::Get().GetModel( "res/models/gltf/simple_skin.gltf" ));
	e3.model.GetRenderModel()->isStatic = false;
	//renderer->ComputeHierarchyR( nullptr, 0, &e3.model.GetRenderModel()->nodes[e3.model.GetRenderModel()->rootNode], glm::mat4( 1.0 ) );
	e3.model.CalculateNodesR( &e3.model.GetRenderModel()->nodes[e3.model.GetRenderModel()->rootNode], glm::mat4( 1.0 ) );
	e3.model.animator.SetAnimation( ResourceManager::Get().GetAnimation( "idle" ) );
	//LoadAnimations("res/models/gltf/imp/anim/imp_run_forward.gltf");
	entites.push_back( e3 );

	Entity e2;
	e2.model.SetRenderModel( ResourceManager::Get().GetModel( "res/models/gltf/sponza/sponza.gltf" ) );
	e2.model.GetRenderModel()->isStatic = true;
	e2.model.CalculateNodesR( &e2.model.GetRenderModel()->nodes[e2.model.GetRenderModel()->rootNode], glm::mat4( 1.0 ) );
	entites.push_back( e2 );

	//Load Skybox
	//ResourceManager::Get().GetTexture( "res/textures/skybox/back.jpg" );
	//ResourceManager::Get().GetTexture( "res/textures/skybox/bottom.jpg" );
	//ResourceManager::Get().GetTexture( "res/textures/skybox/front.jpg" );
	//ResourceManager::Get().GetTexture( "res/textures/skybox/left.jpg" );
	//ResourceManager::Get().GetTexture( "res/textures/skybox/right.jpg" );
	//ResourceManager::Get().GetTexture( "res/textures/skybox/top.jpg" );

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

	Light directional{};
	directional.lType = LIGHT_DIRECTIONAL;
	directional.color = glm::vec3( 1 );
	directional.pos = glm::vec3( -8.3, 14.2, -0.8 );
	directional.direction = glm::vec3( -0.7, -0.7, -0.1 );
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
	spot.pos = glm::vec3( -1.4, 1, 0.0 );
	spot.direction = glm::vec3( 11, 0.1, -0.8 );
	spot.quadratic = 0.019;
	spot.farPlane = 25.0f;
	spot.shadowAtlasLocation.index = glm::ivec2( -1, -1 );
	spot.shadowMapSize = glm::vec2( 1024 );
	spot.hasShadow = true;
	lights.push_back( spot );

	//camera.transform.SetRotation( glm::vec3( 0, 1, 0 ) );
}