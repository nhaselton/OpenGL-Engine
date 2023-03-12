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

void ComputeHierarchy(  Animation* animation, float time,  Node* node, glm::mat4 parent = glm::mat4(1.0) ){
	node->computedOffset = node->t.Matrix();//animation offset
	for ( int i = 0; i < animation->animChannels.size(); i++ ) {

		AnimChannel* anim = &animation->animChannels[i];
		if ( anim->nodeID == node->index && anim->rotations.size() > 0 ){
			Transform t;
			t.SetRotation( glm::eulerAngles(glm::quat(anim->rotations[index].rotaiton)) );
			t.SetPosition( anim->translations[index].translation);
			t.SetScale( anim->scales[index].scale );
			node->computedOffset = t.Matrix();
		}
	}
	
	for ( int i = 0; i < node->children.size(); i++ )
		ComputeHierarchy( animation, index , node->children[i], node->computedOffset );
}

void Renderer::Init( Window* window, Camera* camera ) {
	this->window = window;

	shader = new Shader( "res/shaders/skeletalShader" );
	//shader = new Shader( "res/shaders/staticLitShader" );
	projection = glm::perspective( glm::radians( 90.0f ), 1280.0f / 720.0f, .1f, 100.0f );
	this->camera = camera;
	glEnable( GL_DEPTH_TEST );

	lightShader = new Shader( "res/shaders/lightShader" );


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); ( void ) io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL( window->GetHandle(), true );
	ImGui_ImplOpenGL3_Init( "#version 330" );
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
}

void Renderer::DrawFrame( std::vector<Entity>& entities, std::vector<Light>& lights ) {
	glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // also clear the depth buffer now!
	
	shader->Use();
	shader->SetBool( "showNormalMap", showNormalMap );
	shader->SetBool( "showSpecularMap", showSpecularMap);

	//TODO UNIFORM BUFFERS
	//Send light to shader
	
	int numPointLights = 0;
	int numSpotLights = 0;
	
	for ( int i = 0; i < lights.size(); i++ ) {
		//Only 1 possible directional light
		if ( lights[i].lType == LIGHT_DIRECTIONAL ) {
			shader->SetVec3( "directionalLight.color", lights[i].color );
			shader->SetVec3( "directionalLight.direction", lights[i].direction );
		}

		else if ( lights[i].lType == LIGHT_POINT ) {
			std::string prefix = "pointLights[" + std::to_string( numPointLights++ ) + "].";
			shader->SetVec3( prefix + "color" , lights[i].color );
			shader->SetVec3( prefix + "direction", lights[i].direction);
			shader->SetVec3( prefix + "pos", lights[i].pos);
			shader->SetFloat( prefix + "linear", lights[i].linear );
			shader->SetFloat( prefix + "quadratic", lights[i].quadratic);
			shader->SetFloat( prefix + "cutoff", lights[i].cutoff);
			shader->SetFloat( prefix + "outerCutoff", lights[i].outerCutoff);
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

	shader->SetMat4( "view", camera->GetView() );//camera->GetView() );
	shader->SetMat4( "projection", projection );
	shader->SetVec3( "viewPos", camera->transform.Position() );

	for ( int m = 0; m < entities.size(); m++ ) {
		if ( entities[m].model == nullptr )
			continue;
	
		
		ComputeHierarchy( &entities[m].model->animations[0], 0 , &entities[m].model->nodes[entities[m].model->rootNode]);
		DrawModelR( entities[m].model, &entities[m].model->nodes[entities[m].model->rootNode] );
	}

	// =========== Draw Lights ============= //
	lightShader->Use();
	lightShader->SetMat4( "projection", projection );
	lightShader->SetMat4( "view", camera->GetView());
	
	Mesh* cube = &ResourceManager::Get().GetModel( "res/models/gltf/cube.glb.gltf" )->meshes[0];
	cube->BindVAO();
	for ( int i = 0; i < lights.size(); i++ ) {
		if ( lights[i].lType != LIGHT_DIRECTIONAL ) {
			glm::mat4 pos = glm::translate( glm::mat4( 1.0 ), lights[i].pos );
			glm::mat4 scale = glm::scale( glm::mat4( 1.0 ), glm::vec3( .25f ) );
			glm::mat4 model = pos * scale;
			lightShader->SetMat4( "model", model );
			lightShader->SetVec3( "color", lights[i].color );
			glDrawElements( GL_TRIANGLES, cube->numIndices, GL_UNSIGNED_SHORT, (void*) ( 0 ) );
		}
	}
	shader->Use();

}

#include "Input.h"
void Renderer::DrawModelR(Model* model, Node* node , glm::mat4 parent ) {


	// Convert to model space
	glm::mat4 modelSpace = parent * node->computedOffset ;
	 
	glm::mat4 jointSpace = modelSpace * node->inverseBind;


	if ( node->isJoint ) {
		shader->SetMat4( "bones[" + std::to_string( node->boneID ) + "]" , jointSpace );
	}

	//TODO ADD THE ENTITY TRANSLATION TOO! (probably just make it parent's TRS)
	for ( int i = 0; i < node->meshIndices.size(); i++ ) {
		Mesh* mesh = &model->meshes[node->meshIndices[i]];//node->meshes[i];
		mesh->BindVAO();
		
		shader->SetMat4( "model", glm::mat4(1.0));

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

		// =========== Draw Lights ============= //
		lightShader->Use();
		lightShader->SetMat4( "projection", projection );
		lightShader->SetMat4( "view", camera->GetView() );

		Mesh* cube = &ResourceManager::Get().GetModel( "res/models/gltf/cube.glb.gltf" )->meshes[0];
		cube->BindVAO();
		glm::mat4 _model = node->computedOffset;
		_model *= glm::scale( glm::mat4( 1.0 ), glm::vec3( .1f ) );
		lightShader->SetMat4( "model", _model );
		lightShader->SetVec3( "color" , glm::vec3(1,0,0));
		//glDrawElements( GL_TRIANGLES, cube->numIndices, GL_UNSIGNED_SHORT, ( void* ) ( 0 ) );
	//}
	shader->Use();

	for ( int n = 0; n < node->children.size(); n++ ) {
		DrawModelR( model, node->children[n], modelSpace);
	}

	//exit(0);
}

void Renderer::EndFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
	glfwSwapBuffers( window->GetHandle() );
}


/*
/*
		for ( int i = 0; i < entities[m].model->meshes.size(); i++ ) {
			Mesh* mesh = &entities[m].model->meshes[i];

			mesh->BindVAO();

			glm::mat4 model( 1.0 );
			model = glm::scale( model, entities[m].transform.Scale() * mesh->node->t.Scale() );
			model *= glm::mat4( glm::quat( entities[m].transform.Rotation() ) );
			model *= glm::mat4( glm::quat(mesh->node->t.Rotation()));
			model = glm::translate( model, entities[m].transform.Position() * mesh->node->t.Position() );
			shader->SetMat4( "model", model );

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
		*/
