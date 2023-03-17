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
float bias;


Renderer::Renderer() {
	index = 0; 
	bias = 0;
	showNormalMap = false;
	showSpecularMap = true;


}

void ComputeHierarchy( Animation* animation, float time, Node* node, glm::mat4 parent = glm::mat4( 1.0 ) ) {
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
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

void Renderer::Init( Window* window, Camera* camera ) {
	this->window = window;

	dynamicShader = new Shader( "res/shaders/skeletalShader" );
	staticShader = new Shader( "res/shaders/StaticLitShader" );
	staticShadowShader = new Shader( "res/shaders/staticshadowshader" );
	lightShader = new Shader( "res/shaders/lightShader" );
	debugDepthQuadShader = new Shader( "res/shaders/depthShader" );

//shader = new Shader( "res/shaders/staticLitShader" );
	projection = glm::perspective( glm::radians( 90.0f ), 1280.0f / 720.0f, .1f, 100.0f );
	this->camera = camera;
	glEnable( GL_DEPTH_TEST );



	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); ( void ) io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL( window->GetHandle(), true );
	ImGui_ImplOpenGL3_Init( "#version 330" );

	glGenFramebuffers( 1, &shadowMapFBO );
	// create depth texture
	glGenTextures( 1, &depthMapImage );
	glBindTexture( GL_TEXTURE_2D, depthMapImage );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer( GL_FRAMEBUFFER, shadowMapFBO );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapImage, 0 );
	glDrawBuffer( GL_NONE );
	glReadBuffer( GL_NONE );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );


	staticShader->Use();
	staticShader->SetInt( "albedo", 0 );
	staticShader->SetInt( "normalMap", 1 );
	staticShader->SetInt( "specularMap", 2 );

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
	ImGui::End();

	ImGui::Begin( "Camera info" );
	ImGui::Text( "Position:  (%.1lf, %.1lf, %.1lf)", camera->transform.Position().x, camera->transform.Position().y, camera->transform.Position().z );
	ImGui::Text( "Euler:  (%.1lf, %.1lf, %.1lf)", camera->transform.Rotation().x, camera->transform.Rotation().y, camera->transform.Rotation().z );
	ImGui::Text( "Forward:  (%.1lf, %.1lf, %.1lf)", camera->GetForward().x, camera->GetForward().y, camera->GetForward().z );
	ImGui::End();

	ImGui::Begin( "Frame" );
	ImGui::SliderInt( "frame", &index, 0, 31 );
	ImGui::End();

	ImGui::Begin( "Shadow Bias" );
	ImGui::SliderFloat( "Shadow Bias", &bias, 0, .1f);
	ImGui::End();
}

void renderQuad();
void Renderer::DrawFrame( std::vector<Entity>& entities, std::vector<Light>& lights ) {
	InitLights( lights );

	float near_plane = 0.10f, far_plane = 20.5f;
	//glm::mat4 lightProjection = projection;
	glm::mat4 lightProjection = glm::ortho( -15.0f, 15.0f, -12.0f, 12.0f, near_plane, far_plane );
	//glm::mat4 lightView = glm::lookAt( lights[0].pos, glm::normalize(lights[0].pos + lights[0].direction), glm::vec3( 0.0, 1.0, 0.0 ) );;
	//glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	
	glm::mat4 lightView = glm::lookAt( lights[0].pos, glm::normalize(lights[0].pos + lights[0].direction), glm::vec3(0,1,0) );
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	// Shadow Draw
	glBindFramebuffer( GL_FRAMEBUFFER, shadowMapFBO );
	glViewport( 0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glClear( GL_DEPTH_BUFFER_BIT );
	staticShadowShader->Use();
	staticShadowShader->SetMat4( "lightSpaceMatrix", lightSpaceMatrix );

	glEnable( GL_CULL_FACE);
	glCullFace( GL_FRONT );

	for ( int i = 0; i < entities[0].model->nodes.size(); i++ ) {
		glm::mat4 matrix = entities[0].model->nodes[i].t.Matrix();

		for ( int n = 0; n < entities[0].model->nodes[i].meshIndices.size(); n++ ) {
			Mesh& mesh = entities[0].model->meshes[entities[0].model->nodes[i].meshIndices[n]];
			staticShadowShader->SetMat4( "model", matrix );

			mesh.BindVAO();
			glDrawElements( GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_SHORT, ( void* ) 0 );
		}
	}
	glDisable(GL_CULL_FACE);



	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, 1280, 720 );
	glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	glActiveTexture( GL_TEXTURE4 );
	glBindTexture( GL_TEXTURE_2D, depthMapImage );

		debugDepthQuadShader->Use();
		debugDepthQuadShader->SetFloat( "near_plane", near_plane );
		debugDepthQuadShader->SetFloat( "far_plane", far_plane );
		debugDepthQuadShader->SetInt("depthMap", 4);
	//	renderQuad();
	//	return;
	
	staticShader->Use();
	staticShader->SetMat4( "view", camera->GetView() );
	staticShader->SetVec3( "viewPos", camera->transform.Position() );
	staticShader->SetMat4( "projection", projection );
	staticShader->SetMat4( "lightSpaceMatrix", lightSpaceMatrix );
	staticShader->SetBool( "showNormalMap", showNormalMap );
	staticShader->SetBool( "showSpecularMap", showSpecularMap );
	staticShader->SetInt( "shadowMap", 4 );
	staticShader->SetFloat( "maxBias", bias );

	for ( int i = 0; i < entities[0].model->nodes.size(); i++ ) {
		glm::mat4 matrix = entities[0].model->nodes[i].t.Matrix();
		for ( int n = 0; n < entities[0].model->nodes[i].meshIndices.size(); n++ ) {
			Mesh& mesh = entities[0].model->meshes[entities[0].model->nodes[i].meshIndices[n]];
			staticShader->SetMat4( "model", matrix );
			BindTextures( &mesh );
			mesh.BindVAO();
			glDrawElements( GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_SHORT, ( void* ) 0 );
		}
	}

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

void Renderer::DrawModelR( Model* model, Node* node, glm::mat4 parent ) {
	Shader* shader = ( model->isStatic ) ? staticShader : dynamicShader;
	shader->Use();
	// Convert to model space
	//glm::mat4 modelSpace = parent * node->computedOffset;
	//
	//glm::mat4 jointSpace = modelSpace * node->inverseBind;


	//if ( node->isJoint ) {
	//	shader->SetMat4( "bones[" + std::to_string( node->boneID ) + "]", jointSpace );
	//}

	//TODO ADD THE ENTITY TRANSLATION TOO! (probably just make it parent's TRS)
	for ( int i = 0; i < node->meshIndices.size(); i++ ) {
		Mesh* mesh = &model->meshes[node->meshIndices[i]];//node->meshes[i];
		mesh->BindVAO();

		shader->SetMat4( "model", node->t.Matrix() );

		if ( mesh->diffuseTexture != nullptr ) {
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, mesh->diffuseTexture->textureID );
			shader->SetInt( "albedo", 0 );
		}


		if ( mesh->normalTexture != nullptr ) {
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, mesh->normalTexture->textureID );
			shader->SetInt( "normalMap", 1 );
		}

		if ( mesh->specularTexture != nullptr ) {
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, mesh->specularTexture->textureID );
			shader->SetInt( "specularMap", 2 );
		}

		if ( mesh->numIndices != 0 ) {
			glDrawElements( GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_SHORT, 0 );
		}
		else {
			glDrawArrays( GL_TRIANGLES, 0, mesh->numVertices );
		}

	}
	/*
	// =========== Draw Lights ============= //
	lightShader->Use();
	lightShader->SetMat4( "projection", projection );
	lightShader->SetMat4( "view", camera->GetView() );

	Mesh* cube = &ResourceManager::Get().GetModel( "res/models/gltf/cube.glb.gltf" )->meshes[0];
	cube->BindVAO();
	glm::mat4 _model = node->computedOffset;
	_model *= glm::scale( glm::mat4( 1.0 ), glm::vec3( .1f ) );
	lightShader->SetMat4( "model", _model );
	lightShader->SetVec3( "color", glm::vec3( 1, 0, 0 ) );
	//glDrawElements( GL_TRIANGLES, cube->numIndices, GL_UNSIGNED_SHORT, ( void* ) ( 0 ) );
//}
	*/

	for ( int n = 0; n < node->children.size(); n++ ) {
		DrawModelR( model, node->children[n], glm::mat4( 1.0 ) );
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

			}

			else if ( lights[i].lType == LIGHT_POINT ) {
				std::string prefix = "pointLights[" + std::to_string( numPointLights++ ) + "].";
				shader->SetVec3( prefix + "color", lights[i].color );
				shader->SetVec3( prefix + "direction", lights[i].direction );
				shader->SetVec3( prefix + "pos", lights[i].pos );
				shader->SetFloat( prefix + "linear", lights[i].linear );
				shader->SetFloat( prefix + "quadratic", lights[i].quadratic );
				shader->SetFloat( prefix + "cutoff", lights[i].cutoff );
				shader->SetFloat( prefix + "outerCutoff", lights[i].outerCutoff );
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
			}

		}

		shader->SetInt( "numPointLights", numPointLights );
		shader->SetInt( "numSpotLights", numSpotLights );
	}
}


void Renderer::ShadowDrawModelR( Model* model, Node* node, glm::mat4 parent ) {
	Shader* shader = staticShadowShader;
	shader->Use();

	// Convert to model space
	for ( int i = 0; i < node->meshIndices.size(); i++ ) {
		Mesh* mesh = &model->meshes[node->meshIndices[i]];//node->meshes[i];
		mesh->BindVAO();
		shader->SetMat4( "model", glm::mat4( 1.0 ) );

		if ( mesh->numIndices != 0 ) {
			glDrawElements( GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_SHORT, 0 );
		}
		else {
			glDrawArrays( GL_TRIANGLES, 0, mesh->numVertices );
		}

		for ( int n = 0; n < node->children.size(); n++ ) {
			ShadowDrawModelR( model, node->children[n], glm::mat4( 1.0 ) );
		}
	}
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if ( quadVAO == 0 )
	{
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


/*


	/*
	//BASIC TEST

	return;
	InitLights( lights );

	dynamicShader->Use();
	dynamicShader->SetMat4( "view", camera->GetView() );//camera->GetView() );
	dynamicShader->SetMat4( "projection", projection );
	dynamicShader->SetVec3( "viewPos", camera->transform.Position() );
	dynamicShader->SetBool( "showNormalMap", showNormalMap );
	dynamicShader->SetBool( "showSpecularMap", showSpecularMap );

	staticShader->Use();
	staticShader->SetBool( "showNormalMap", showNormalMap );
	staticShader->SetBool( "showSpecularMap", showSpecularMap );
	staticShader->SetMat4( "view", camera->GetView() );//camera->GetView() );
	staticShader->SetMat4( "projection", projection );
	staticShader->SetVec3( "viewPos", camera->transform.Position() );


	float near_plane = .1f, far_plane = 20.0f;
	glm::mat4 lightProjection = glm::ortho( -20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane );



	//glm::mat4 lightView = glm::lookAt( glm::vec3( -2.0f, 4.0f, -1.0f ),
	//	glm::vec3( 0.0f, 0.0f, 0.0f ),
	//	glm::vec3( 0.0f, 1.0f, 0.0f ) );

	//glm::mat4 lightView = glm::lookAt( lights[0].pos, lights[0].pos + lights[0].direction, glm::vec3( 0, 1, 0 ) );


	glm::vec3 directonal = lights[0].direction;
	directonal.x = cos( directonal.y ) * cos( directonal.x );
	directonal.y = sin( directonal.x );
	directonal.z = sin( directonal.y ) * cos( directonal.x );
	directonal = glm::normalize( directonal );

	//Lookat
	glm::mat4 lightView = glm::lookAt( lights[0].pos, lights[0].pos + directonal, glm::vec3( 0, 1, 0 ) );

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;


	// ==================================  //
	// ======== SHADOW PASS ============= //
	// ================================== //
	glBindFramebuffer( GL_FRAMEBUFFER, shadowMapFBO );
	glClear( GL_DEPTH_BUFFER_BIT );
	staticShadowShader->Use();
	staticShadowShader->SetMat4( "lightSpaceMatrix", lightSpaceMatrix );
	ComputeHierarchy( nullptr, 0, &entities[0].model->nodes[entities[0].model->rootNode] );
	ShadowDrawModelR( entities[0].model, &entities[0].model->nodes[entities[0].model->rootNode] );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // also clear the depth buffer now!

	staticShader->Use();
	staticShader->SetMat4( "lightSpaceMatrix", lightSpaceMatrix );
	staticShader->SetInt( "shadowMap", 4 );
	glActiveTexture( GL_TEXTURE4 );
	glBindTexture( GL_TEXTURE_2D, depthMapImage );
	//glBindTexture( GL_TEXTURE_2D, buffers.depthMapImage );
	// ================= //
	// ====== DRAW ===== //
	// ================= //


	//debugDepthQuadShader->Use();
	//debugDepthQuadShader->SetInt( "depthMap", 4 );
	//debugDepthQuadShader->SetFloat( "near_plane", near_plane );
	//debugDepthQuadShader->SetFloat( "far_plane", far_plane);
	//glActiveTexture( GL_TEXTURE4 );
	//glBindTexture( GL_TEXTURE_2D, depthMapImage );
	//renderQuad( );
	//return;


	for ( int m = 0; m < entities.size(); m++ ) {
		if ( entities[m].model == nullptr )
			continue;
		if ( entities[m].model->isStatic ) {
			ComputeHierarchy( nullptr, 0, &entities[m].model->nodes[entities[m].model->rootNode] );
			DrawModelR( entities[m].model, &entities[m].model->nodes[entities[m].model->rootNode] );
		}
		else {
			Animation* a;
			a = ResourceManager::Get().GetAnimation( "idle" );
			a = ResourceManager::Get().GetAnimation( "run_forward.003" );
			//			a = nullptr;
			ComputeHierarchy( a, 0, &entities[m].model->nodes[entities[m].model->rootNode] );
			DrawModelR( entities[m].model, &entities[m].model->nodes[entities[m].model->rootNode], glm::scale( glm::mat4( 1.0 ), glm::vec3( .5f ) ) );
		}
	}
	*/
