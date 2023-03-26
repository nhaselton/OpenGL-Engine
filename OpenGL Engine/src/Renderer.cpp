#include "Renderer.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "glm/gtx/projection.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "Window.h"
#include "Shader.h"
#include "ResourceManager.h" 

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int index;

Renderer::Renderer() {
	index = 0;
	showNormalMap = true;
	showSpecularMap = true;
}

//const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

void Renderer::Init( Window* window, Camera* camera ) {
	CreateShadowAtlas();

	this->window = window;
	dynamicShader = new Shader( "res/shaders/skeletalShader/skeletalShader.vs", "res/shaders/StaticLitShader/StaticLitShader.fs" );
	staticShader = new Shader( "res/shaders/StaticLitShader" );
	staticShadowShader = new Shader( "res/shaders/staticshadowshader" );
	debugDepthQuadShader = new Shader( "res/shaders/depthShader" );
	staticShadowCubeMapAtlasShader = new Shader( "res/shaders/CubeDepthAtlasShader" );
	dynamicShadowCubeMapAtlasShader = new Shader( "res/shaders/CubeDepthAtlasShader" );
	staticDepthPrepassShader = new Shader( "res/shaders/staticdepthpass" );
	dynamicShadowShader = new Shader( "res/shaders/DynamicShadowShader" );

	projection = glm::perspective( glm::radians( 90.0f ), 1280.0f / 720.0f, .1f, 100.0f );
	this->camera = camera;
	glEnable( GL_DEPTH_TEST );

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); ( void ) io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL( window->GetHandle(), true );
	ImGui_ImplOpenGL3_Init( "#version 330" );

	staticShader->Use();
	staticShader->SetInt( "albedo", 0 );
	staticShader->SetInt( "normalMap", 1 );
	staticShader->SetInt( "specularMap", 2 );
	staticShader->SetInt( "shadowMap", 4 );
	staticShader->SetInt( "cubeMap", 5 );
	staticShader->SetInt( "shadowAtlas", 15 );
	staticShader->SetMat4( "projection", projection );

	dynamicShader->Use();
	dynamicShader->SetInt( "albedo", 0 );
	dynamicShader->SetInt( "normalMap", 1 );
	dynamicShader->SetInt( "specularMap", 2 );
	dynamicShader->SetInt( "shadowMap", 4 );
	dynamicShader->SetInt( "cubeMap", 5 );
	dynamicShader->SetInt( "shadowAtlas", 15 );
	dynamicShader->SetMat4( "projection", projection );

	staticDepthPrepassShader->Use();
	staticDepthPrepassShader->SetMat4( "projection", projection );

	int numSquares = ( SHADOW_ATLAS_WIDTH * SHADOW_ATLAS_HEIGHT) / ( SHADOW_ATLAS_TILE_SIZE * SHADOW_ATLAS_TILE_SIZE );//this will always divide equally into int
	int numInts = numSquares / 32;

	shadowAtlasContents = ( unsigned int* ) malloc( numInts * sizeof( int ) );
	memset( shadowAtlasContents, 0, numInts * sizeof( int ) );
}


//TODO CHECK CUBE MAP BOUNDS BETTER LATER
void Renderer::CreateShadowAtlas() {
	glGenFramebuffers( 1, &shadowAtlasFBO );

	glGenTextures( 1, &shadowAtlasImage );
	glBindTexture( GL_TEXTURE_2D, shadowAtlasImage );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_ATLAS_WIDTH, SHADOW_ATLAS_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
	float borderColor[] = { 0, 0, 0, 1.0f };
	glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );

	glBindFramebuffer( GL_FRAMEBUFFER, shadowAtlasFBO );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowAtlasImage, 0 );
	glDrawBuffer( GL_NONE );
	glReadBuffer( GL_NONE );

	glClearColor( FLT_MAX, 0, 0, 0 );
	glClear( GL_DEPTH_BUFFER_BIT );

	glActiveTexture( GL_TEXTURE15 );
	glBindTexture( GL_TEXTURE_2D, shadowAtlasImage );

	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		std::cout << "Shadow atlas frame buffer not complete";

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	//bind image here, it never gets unbound because 15 is reserved for it
	glActiveTexture( GL_TEXTURE15 );
	glBindTexture( GL_TEXTURE_2D, shadowAtlasImage );
}

void Renderer::BeginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin( "fps" );
	ImGui::Text( "Fps: %.0f        Ms: %.2f", ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime * 1000 );
	ImGui::End();

	ImGui::Begin( "Show Normals" );
	ImGui::Checkbox( "use NormalMap?", &showNormalMap );
	ImGui::Checkbox( "use SpecularMap?", &showSpecularMap );
	ImGui::Checkbox( "Show Shadow Atlas?", &showShadowAtlas );
	ImGui::End();

	ImGui::Begin( "Camera info" );
	ImGui::Text( "Position:  (%.1lf, %.1lf, %.1lf)", camera->transform.Position().x, camera->transform.Position().y, camera->transform.Position().z );
	ImGui::Text( "Euler:  (%.1lf, %.1lf, %.1lf)", camera->transform.Rotation().x, camera->transform.Rotation().y, camera->transform.Rotation().z );
	ImGui::Text( "Forward:  (%.1lf, %.1lf, %.1lf)", camera->GetForward().x, camera->GetForward().y, camera->GetForward().z );
	ImGui::End();

	ImGui::Begin( "Frame" );
	ImGui::SliderInt( "frame", &index, 0, 31 );
	ImGui::End();
}

#include "Input.h"

void Renderer::DrawFrame( std::vector<Entity>& entities, std::vector<Light>& lights ) {
	//for some reason this gets unbound
	glActiveTexture( GL_TEXTURE15 );
	glBindTexture( GL_TEXTURE_2D, shadowAtlasImage );

	glBindFramebuffer( GL_FRAMEBUFFER, shadowAtlasFBO );
	glClear( GL_DEPTH_BUFFER_BIT );

	DrawPointLight( lights[0], entities );
	DrawDirectionalLight( lights[1], entities );
	DrawSpotLight( lights[2], entities );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, 1280, 720 );
	glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	if ( showShadowAtlas ) {
		debugDepthQuadShader->Use();
		debugDepthQuadShader->SetFloat( "near_plane", 1 );
		debugDepthQuadShader->SetInt( "depthMap", 15 );
		debugDepthQuadShader->SetFloat( "far_plane", 25 );
		renderQuad();
		return;
	}

	// == DEPTH PREPASS == // 
	staticDepthPrepassShader->Use();
	staticDepthPrepassShader->SetMat4( "view", camera->GetView() );
	DrawModelR( staticDepthPrepassShader, entities[0].model, &entities[0].model->nodes[entities[0].model->rootNode], false, glm::mat4( 1.0 ) );
	glDepthFunc( GL_LEQUAL );

	//TODO init singular light function
	InitLights( lights );
	staticShader->Use();
	staticShader->SetMat4( "view", camera->GetView() );
	staticShader->SetBool( "showNormalMap", showNormalMap );
	staticShader->SetBool( "showSpecularMap", showSpecularMap );
	staticShader->SetVec3( "viewPos", camera->transform.Position() );

	dynamicShader->Use();
	dynamicShader->SetMat4( "view", camera->GetView() );
	dynamicShader->SetBool( "showNormalMap", showNormalMap );
	dynamicShader->SetBool( "showSpecularMap", showSpecularMap );
	dynamicShader->SetVec3( "viewPos", camera->transform.Position() );



	for ( int i = 0; i < entities.size(); i++ ) {
		Shader* shader = ( entities[i].model->isStatic ) ? staticShader : dynamicShader;
		shader->Use();
		Model* model = entities[i].model;
		if ( !model->isStatic )
			ComputeHierarchy( ResourceManager::Get().GetAnimation("idle"), index, &model->nodes[model->rootNode], glm::scale(glm::mat4(1.0), glm::vec3(1.0)));

		DrawModelR( shader, entities[i].model, &entities[i].model->nodes[entities[i].model->rootNode], true , glm::scale(glm::mat4(1.0),glm::vec3(.5f)));
	}
	glDepthFunc( GL_LESS );

	if ( Input::keys[GLFW_KEY_SPACE] ) {
		lights[0].pos = camera->transform.Position();
		lights[0].direction = camera->GetForward();
	}
}

//wonder if i could figure out how to do this with 1 viewport, find start pos, then i could get bounds and maybe draw correctly
void Renderer::DrawPointLight( Light& light, std::vector<Entity>& entities ) {
	glBindFramebuffer( GL_FRAMEBUFFER, shadowAtlasFBO );

	if ( light.shadowAtlasLocation.index.x == -1 && light.hasShadow) {
		light.shadowAtlasLocation = FindFreeSpaceInShadowAltas( SHADOW_MAP_CUBE, light.shadowMapSize.x, light.shadowMapSize.y );
	}

	float aspect = ( float ) light.shadowMapSize.x / ( float ) light.shadowMapSize.y;
	float near = 1.0f;
	glm::mat4 shadowProj = glm::perspective( glm::radians( 90.0f ), aspect, near, light.farPlane );
	std::vector<glm::mat4> shadowTransforms;
	glm::vec3 lightPos = light.pos;

	shadowTransforms.push_back( shadowProj * glm::lookAt( lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0 ), glm::vec3( 0.0, -1.0, 0.0 ) ) );
	shadowTransforms.push_back( shadowProj * glm::lookAt( lightPos, lightPos + glm::vec3( -1.0, 0.0, 0.0 ), glm::vec3( 0.0, -1.0, 0.0 ) ) );
	shadowTransforms.push_back( shadowProj * glm::lookAt( lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0 ), glm::vec3( 0.0, 0.0, 1.0 ) ) );
	shadowTransforms.push_back( shadowProj * glm::lookAt( lightPos, lightPos + glm::vec3( 0.0, -1.0, 0.0 ), glm::vec3( 0.0, 0.0, -1.0 ) ) );
	shadowTransforms.push_back( shadowProj * glm::lookAt( lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0 ), glm::vec3( 0.0, -1.0, 0.0 ) ) );
	shadowTransforms.push_back( shadowProj * glm::lookAt( lightPos, lightPos + glm::vec3( 0.0, 0.0, -1.0 ), glm::vec3( 0.0, -1.0, 0.0 ) ) );

	/*
		+Y
	 -X +Z +X -Z
		-Y
	*/

	float size = light.shadowMapSize.x;
	int xLoc = light.shadowAtlasLocation.index.x;
	int yLoc = light.shadowAtlasLocation.index.y;

	dynamicShadowCubeMapAtlasShader->Use();
	dynamicShadowCubeMapAtlasShader->SetFloat( "far_plane", light.farPlane );
	dynamicShadowCubeMapAtlasShader->SetVec3( "lightPos", light.pos );
	
	staticShadowCubeMapAtlasShader->Use();
	staticShadowCubeMapAtlasShader->SetFloat( "far_plane", light.farPlane );
	staticShadowCubeMapAtlasShader->SetVec3( "lightPos", light.pos );


	int tileSize = light.shadowMapSize.x / SHADOW_ATLAS_TILE_SIZE;

	//sets the dynamci and static light var
	auto SetLightSpace([]( Shader* a, Shader* b,glm::mat4 val ) {
		a->Use();
		a->SetMat4( "lightSpaceMatrix", val );
		b->Use();
		b->SetMat4( "lightSpaceMatrix", val );
	});

	glViewport( xLoc * SHADOW_ATLAS_TILE_SIZE, yLoc * SHADOW_ATLAS_TILE_SIZE, size, size );
	SetLightSpace( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, shadowTransforms[4] );
	DrawScene( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, false, entities );
	
	//Top
	glViewport( ( xLoc + 0 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, ( yLoc - 1 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, size, size );
	SetLightSpace( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, shadowTransforms[2] );
	DrawScene( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, false, entities );

	//Bottom
	glViewport( ( xLoc + 0 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, ( yLoc + 1 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, size, size );
	SetLightSpace( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, shadowTransforms[3] );
	DrawScene( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, false, entities );

	//Left
	glViewport( ( xLoc - 1 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, ( yLoc + 0 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, size, size );
	SetLightSpace( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, shadowTransforms[1] );
	DrawScene( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, false, entities );

	//Right 1
	glViewport( ( xLoc + 1 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, ( yLoc + 0 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, size, size );
	SetLightSpace( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, shadowTransforms[0] );
	DrawScene( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, false, entities );
	
	//Right 2
	glViewport( ( xLoc + 2 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, ( yLoc + 0 * tileSize ) * SHADOW_ATLAS_TILE_SIZE, size, size );
	SetLightSpace( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, shadowTransforms[5] );
	DrawScene( staticShadowCubeMapAtlasShader, dynamicShadowCubeMapAtlasShader, false, entities );
}

void Renderer::DrawSpotLight( Light& light, std::vector<Entity>& entities ) {
	glBindFramebuffer( GL_FRAMEBUFFER, shadowAtlasFBO );

	if ( light.shadowAtlasLocation.index.x == -1 && light.hasShadow ) {
		light.shadowAtlasLocation = FindFreeSpaceInShadowAltas( SHADOW_MAP_OMNIDIRECTIONAL, light.shadowMapSize.x, light.shadowMapSize.y );
	}

	int texX = light.shadowAtlasLocation.index.x;
	int texY = light.shadowAtlasLocation.index.y;
	glViewport( texX * SHADOW_ATLAS_TILE_SIZE, texY * SHADOW_ATLAS_TILE_SIZE, light.shadowMapSize.x, light.shadowMapSize.y );

	float near_plane = 1.0f, far_plane = light.farPlane;
	
	glm::mat4 lightProjection = glm::perspective( glm::radians( 90.0f ), ( float ) light.shadowMapSize.x / ( float ) light.shadowMapSize.y, near_plane, far_plane );
	glm::mat4 view = glm::lookAt( light.pos, light.pos + light.direction, glm::vec3( 0, 1, 0 ) );
	glm::mat4 lightSpaceMatrix = lightProjection * view;

	staticShadowShader->Use();
	staticShadowShader->SetMat4( "lightSpaceMatrix", lightSpaceMatrix );
	dynamicShadowShader->Use();
	dynamicShadowShader->SetMat4( "lightSpaceMatrix", lightSpaceMatrix );
	light.lightSpaceMatrix = lightSpaceMatrix;

	DrawScene( staticShadowShader, dynamicShadowShader, false, entities );
}


void Renderer::DrawScene( Shader* staticShader, Shader* dynamicShader, bool drawTextures, std::vector<Entity>& entities ) {
	for ( int i = 0; i < entities.size(); i++ ) {
		Shader* shader = ( entities[i].model->isStatic ) ? staticShader : dynamicShader;
		shader->Use();
		DrawModelR( shader, entities[i].model, &entities[i].model->nodes[entities[i].model->rootNode], false, glm::scale( glm::mat4( 1.0 ), glm::vec3( .5f ) ) );
	}
}


//Keep these as unclassed functions for lambda
static bool checkBit( unsigned int* data, int bit ) {
	int intIndx = bit / 32; //get which int its in
	int bitIndx = bit % 32; //get offset
	return bitcheck( data[intIndx], bitIndx );
}

static void setBit( unsigned int* data, int bit, int val ) {
	int intIndx = bit / 32; //get which int its in
	int bitIndx = bit % 32; //get offset
	
	if ( val == 0 )
		bitclear( data[intIndx], bitIndx );
	else if ( val == 1 )
		bitset( data[intIndx], bitIndx );
	else
		std::cout << "[WARNING] invalid set bit: " << val << std::endl;
}


AtlasLocation Renderer::FindFreeSpaceInShadowAltas( shadowMapType type, int shadowWidth, int shadowHeight ) {
	static int totalNumTilesX = SHADOW_ATLAS_WIDTH  / SHADOW_ATLAS_TILE_SIZE;
	static int totalNumTilesY = SHADOW_ATLAS_HEIGHT / SHADOW_ATLAS_TILE_SIZE;

	int tilesNeededX = shadowWidth / 512;
	int tilesNeededY = shadowHeight / 512;

	//Go through each tile
		//if tile is empty
			//check numTilesX -1 to the right
			//check numTilesY - 1 down
				//if all are free then set them 
	auto checkSquare = [] ( unsigned int* shadowAtlasContents, int boardWidth, int x, int y, int w, int h, unsigned int locations[100], int* numInts ) {
		for ( int j = 0; j < h; j++ )
			for ( int n = 0; n < w; n++ ) {
				int loc = y * boardWidth + x;//TODO REMOVE CONSTNAT
				int plusX = n;
				int plusY = totalNumTilesY * j;
				int newLoc = loc + plusY + plusX;
				locations[*numInts] = newLoc;
				( *numInts )++;
				if ( checkBit( shadowAtlasContents, newLoc ) == 1 ) {
					return  false;
				}
			}
		return true;
	};

	bool done = false;
	unsigned int locations[100];
	int numLocations = 0;

	//cube map and omnidirectional start and differnet X & Ys
	int startX = ( type == SHADOW_MAP_CUBE ) ? tilesNeededX : 0;
	int endX = ( type == SHADOW_MAP_CUBE ) ? totalNumTilesX - ( tilesNeededX * 2 ) : totalNumTilesX - (tilesNeededX-1);

	int startY = ( type == SHADOW_MAP_CUBE ) ? tilesNeededY : 0;
	int endY = ( type == SHADOW_MAP_CUBE ) ? totalNumTilesY - ( tilesNeededX - 1 ) : totalNumTilesY - ( tilesNeededY - 1 );

	for ( int y = startY; y < endY ; y++ )
		for ( int x = startX; x < endX; x++ ) {
			if ( done ) break;
			int loc = ( y * totalNumTilesX + x );
			if ( checkBit( shadowAtlasContents, loc ) == 0 ) {
				numLocations = 0;
				bool isSpace = true;
				if ( type == SHADOW_MAP_CUBE ) {
					isSpace = (
						checkSquare( shadowAtlasContents, totalNumTilesX, x, y, tilesNeededX, tilesNeededY, locations, &numLocations ) &&
						checkSquare( shadowAtlasContents, totalNumTilesX, x - tilesNeededX, y, tilesNeededX, tilesNeededY, locations, &numLocations ) &&
						checkSquare( shadowAtlasContents, totalNumTilesX, x + tilesNeededX, y, tilesNeededX, tilesNeededY, locations, &numLocations ) &&
						checkSquare( shadowAtlasContents, totalNumTilesX, x + ( tilesNeededX * 2 ), y, tilesNeededX, tilesNeededY, locations, &numLocations ) &&
						checkSquare( shadowAtlasContents, totalNumTilesX, x, y - tilesNeededY, tilesNeededX, tilesNeededY, locations, &numLocations ) &&
						checkSquare( shadowAtlasContents, totalNumTilesX, x, y + tilesNeededY, tilesNeededX, tilesNeededY, locations, &numLocations )
						);
				}
				else if ( type == SHADOW_MAP_OMNIDIRECTIONAL ) {
					isSpace = ( checkSquare( shadowAtlasContents, totalNumTilesX, x, y, tilesNeededX, tilesNeededY, locations, &numLocations ) );
				}


				if ( isSpace ) {
					done = true;
					for ( int i = 0; i < numLocations; i++ )
						setBit( shadowAtlasContents, locations[i], 1 );

					//Get UV coords
					//16x16
					float minX = ( float ) x / 16.0f;
					float minY = ( float ) y / 16.0f;
					float maxX = ( float ) ( x + tilesNeededX ) / 16.0f;
					float maxY = ( float ) ( y + tilesNeededY ) / 16.0f;
					AtlasLocation loc;
					loc.texCoords = glm::vec4( minX, minY, maxX, maxY );
					loc.index = glm::ivec2( x, y );

					return loc;
				}
			}
		}

	std::cout << "COULD NOT FIND A FREE LOCATION, ATLAS LOOKS LIKE" << std::endl;
	DebugPrintShadowAtlas();
	AtlasLocation l;
	memset( &l, -1, sizeof( AtlasLocation ) );
	return l;
}

void Renderer::DebugPrintShadowAtlas() {
	for ( int y = 0; y < 8192 / 512; y++ ) {
		for ( int x = 0; x < 8192 / 512; x++ ) {
			std::cout << checkBit( shadowAtlasContents, y * ( 8192 / 512 ) + x );
		}
		std::cout << std::endl;
	}
}


void Renderer::DrawDirectionalLight( Light& light, std::vector<Entity>& entities ) {
	glBindFramebuffer( GL_FRAMEBUFFER, shadowAtlasFBO );

	//Check if it is in atlas, if not add it
	if ( light.shadowAtlasLocation.index.x == -1 && light.hasShadow ) {
		AtlasLocation loc = FindFreeSpaceInShadowAltas( SHADOW_MAP_OMNIDIRECTIONAL, light.shadowMapSize.x, light.shadowMapSize.y );
		light.shadowAtlasLocation = loc;
	}

	int texX = light.shadowAtlasLocation.index.x;
	int texY = light.shadowAtlasLocation.index.y;
	glViewport( texX * SHADOW_ATLAS_TILE_SIZE, texY * SHADOW_ATLAS_TILE_SIZE, light.shadowMapSize.x, light.shadowMapSize.y );

	float near_plane = 1.0f, far_plane = light.farPlane;
	glm::mat4 lightProjection = glm::ortho( -15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane );
	glm::mat4 view = glm::lookAt( light.pos, light.pos + light.direction, glm::vec3( 0, 1, 0 ) );

	glm::mat4 lightSpaceMatrix = lightProjection * view;
	light.lightSpaceMatrix = lightSpaceMatrix;
	
	staticShadowShader->Use();
	staticShadowShader->SetMat4( "lightSpaceMatrix", lightSpaceMatrix );
	dynamicShadowShader->Use();
	dynamicShadowShader->SetMat4( "lightSpaceMatrix", lightSpaceMatrix );
	light.lightSpaceMatrix = lightSpaceMatrix;
	DrawScene( staticShadowShader, dynamicShadowShader, false, entities );
	//DrawModelR( staticShadowShader, entities[0].model, &entities[0].model->nodes[entities[0].model->rootNode], false, glm::mat4( 1.0 ) );
}


void Renderer::BindTextures( Mesh* mesh ) {
	if ( mesh->diffuseTexture != nullptr ) {
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, mesh->diffuseTexture->textureID );
	}
	if ( mesh->normalTexture != nullptr ) {
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, mesh->normalTexture->textureID );
	}

	if ( mesh->specularTexture != nullptr ) {
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, mesh->specularTexture->textureID );
	}
}

void Renderer::DrawModelR( Shader* shader, Model* model, Node* node, bool shouldTexture, glm::mat4 parent ) {
	shader->Use();
	// Convert to model space
	glm::mat4 modelSpace = parent * node->computedOffset;
	glm::mat4 jointSpace = modelSpace * node->inverseBind;


	if ( node->isJoint ) {
		shader->SetMat4( "bones[" + std::to_string( node->boneID ) + "]", jointSpace );
	}

	for ( int i = 0; i < node->meshIndices.size(); i++ ) {
		Mesh* mesh = &model->meshes[node->meshIndices[i]];//node->meshes[i];
		mesh->BindVAO();

		shader->SetMat4( "model", node->t.Matrix() );

		if ( shouldTexture )
			BindTextures( mesh );


		if ( mesh->numIndices != 0 ) {
			glDrawElements( GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_SHORT, 0 );
		}
		else {
			glDrawArrays( GL_TRIANGLES, 0, mesh->numVertices );
		}
	}

	for ( int n = 0; n < node->children.size(); n++ ) {
		DrawModelR( shader, model, node->children[n], shouldTexture, modelSpace );
	}
}

void Renderer::EndFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
	glfwSwapBuffers( window->GetHandle() );
}


void Renderer::InitLights( std::vector<Light> lights ) {
	Shader* shaders[] = {
		staticShader,
		dynamicShader
	};
	int numShaders = 2;

	for ( int s = 0; s < numShaders; s++ ) {
		Shader* shader = shaders[s];
		shader->Use();
		int numPointLights = 0;
		int numSpotLights = 0;

		for ( int i = 0; i < lights.size(); i++ ) {
			//Only 1 possible directional light
			if ( lights[i].lType == LIGHT_DIRECTIONAL ) {
				shader->SetVec3( "directionalLight.color", lights[i].color );
				shader->SetVec3( "directionalLight.pos", lights[i].pos );
				shader->SetVec3( "directionalLight.direction", lights[i].direction );
				shader->SetVec4( "directionalLight.shadowUVs", lights[i].shadowAtlasLocation.texCoords );
				shader->SetMat4( "directionalLightSpaceMatrix", lights[i].lightSpaceMatrix );
			}

			else if ( lights[i].lType == LIGHT_POINT ) {
				std::string prefix = "pointLights[" + std::to_string( numPointLights++ ) + "].";
				shader->SetVec3( prefix + "color", lights[i].color );
				shader->SetVec3( prefix + "direction", lights[i].direction );
				shader->SetVec3( prefix + "pos", lights[i].pos );
				shader->SetFloat( prefix + "linear", lights[i].linear );
				shader->SetFloat( prefix + "quadratic", lights[i].quadratic );
				shader->SetFloat( prefix + "cutoff", lights[i].cutoff );
				shader->SetFloat( prefix + "farPlane", lights[i].farPlane );
				shader->SetFloat( prefix + "outerCutoff", lights[i].outerCutoff );
				shader->SetVec4( prefix + "shadowUVs", lights[i].shadowAtlasLocation.texCoords );
				shader->SetInt( prefix + "ID", numPointLights - 1 );
			}

			else if ( lights[i].lType == LIGHT_SPOT ) {
				std::string prefix = "spotLights[" + std::to_string( numSpotLights++ ) + "].";
				shader->SetVec3( prefix + "color", lights[i].color );
				shader->SetVec3( prefix + "direction", lights[i].direction );
				shader->SetVec3( prefix + "pos", lights[i].pos );
				shader->SetFloat( prefix + "linear", lights[i].linear );
				shader->SetFloat( prefix + "quadratic", lights[i].quadratic );
				shader->SetFloat( prefix + "cutoff", lights[i].cutoff );
				shader->SetFloat( prefix + "outerCutoff", lights[i].outerCutoff );
				shader->SetVec4( prefix + "shadowUVs", lights[i].shadowAtlasLocation.texCoords );
				shader->SetMat4( "spotLightSpaceMatrices[" + std::to_string( numSpotLights - 1 ) + "]", lights[i].lightSpaceMatrix );
				shader->SetInt( prefix + "ID", numSpotLights - 1 );
			}

		}

		shader->SetInt( "numPointLights", numPointLights );
		shader->SetInt( "numSpotLights", numSpotLights );
	}
}

void Renderer::renderQuad() {
	if ( quadVAO == 0 ) {
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays( 1, &quadVAO );
		glGenBuffers( 1, &quadVBO );
		glBindVertexArray( quadVAO );
		glBindBuffer( GL_ARRAY_BUFFER, quadVBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), &quadVertices, GL_STATIC_DRAW );
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* ) 0 );
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* ) ( 3 * sizeof( float ) ) );
	}
	glBindVertexArray( quadVAO );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	glBindVertexArray( 0 );
}

void Renderer::ComputeHierarchy( Animation* animation, float time, Node* node, glm::mat4 parent ) {
	node->computedOffset = node->t.Matrix();//animation offset
	if ( animation ) {

		for ( int i = 0; i < animation->animChannels.size(); i++ ) {
			AnimChannel* anim = &animation->animChannels[i];
			if ( anim->nodeID == node->index && anim->rotations.size() > 0 ) {
				Transform t;
				t.SetRotation( glm::eulerAngles( glm::quat( anim->rotations[index].rotaiton ) ) );
				t.SetPosition( anim->translations[index].translation );
				t.SetScale( anim->scales[index].scale );
				node->computedOffset = t.Matrix();

				if ( node->name == "origin" )
					node->computedOffset = node->t.Matrix();
			}
		}
	}

	for ( int i = 0; i < node->children.size(); i++ )
		ComputeHierarchy( animation, index, node->children[i], node->computedOffset );
}
