#include "Tools.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include <json.hpp>

#include <glm/glm.hpp>
#include <stb/stb_image.h>
#include <glm/gtx/string_cast.hpp>


#include "ResourceManager.h"
#include "Texture.h"
typedef nlohmann::json JSON;

enum GLTF_TYPE {
	GLTF_BYTE = 5120,
	GLTF_UBYTE = 5121,
	GLTF_SHORT = 5122,
	GLTF_USHORT = 5123,
	GLTF_UINT = 5125,
	GLTF_FLOAT = 5126
};

struct Accessor {
	unsigned int bufferView;
	GLTF_TYPE componentType;
	unsigned int count;
	std::string type;
};
struct Material {
	int baseColorTexture;
	int metallicRoughnessTexture;
	int normalTexture;
};

std::string FileContentToString( std::string path );

Accessor GetAccessor( JSON json ) {
	Accessor accessor{};
	accessor.bufferView = json["bufferView"];
	accessor.componentType = json["componentType"];
	accessor.count = json["count"];
	accessor.type = json["type"];
	return accessor;
}

std::string GetBinDataFromAccessor( Accessor& accessor, JSON json, std::string& binContents ) {
	int byteLength = json["byteLength"];
	int offset = json["byteOffset"];
	std::string data = binContents.substr( offset, byteLength );
	return data;
}

Model LoadStaticModelGLTF( const char* _path ) {
	Model model;
	model.isStatic = true;

	std::string path = _path;
	std::string fileContent = FileContentToString( path );
	JSON json = JSON::parse( fileContent );

	std::string directory = path.substr( 0, path.find_last_of( "/" ) + 1 );
	std::string binPath = directory;
	binPath += json["buffers"][0]["uri"];
	unsigned int binSize = json["buffers"][0]["byteLength"];

	std::string binContents = FileContentToString( binPath );


	//Load Materials

	std::vector<Material> materials;
	for ( int i = 0; i < json["materials"].size(); i++ ) {
		JSON matJson = json["materials"][i];
		Material mat{ -1 };
		mat.baseColorTexture = -1;
		mat.metallicRoughnessTexture = -1;
		mat.normalTexture = -1;

		if ( matJson["pbrMetallicRoughness"].find( "baseColorTexture" ) != matJson["pbrMetallicRoughness"].end() ) {
			mat.baseColorTexture = matJson["pbrMetallicRoughness"]["baseColorTexture"]["index"];
		}


		if ( matJson["pbrMetallicRoughness"].find( "metallicRoughnessTexture" ) != matJson["pbrMetallicRoughness"].end() ) {
			mat.metallicRoughnessTexture = matJson["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"];
		}

		if ( matJson.find( "normalTexture" ) != matJson.end() ) {
			mat.normalTexture = matJson["normalTexture"]["index"];
		}

		materials.push_back( mat );
	}

	// ============
	//	CREATE NODES
	// ============
	JSON accessorJson = json["accessors"];
	//TODO create node hierechy from this
	model.rootNode = json["scenes"][0]["nodes"][0];

	model.nodes.resize( json["nodes"].size() );// resize here so

	for ( int i = 0; i < json["nodes"].size(); i++ ) {
		JSON nodeJson = json["nodes"][i];
		Node n;
		memset( &n, 0, sizeof( Node ) );
		n.t.SetScale( glm::vec3( 1 ) );

		if ( nodeJson.find( "name" ) != nodeJson.end() ) {
			n.name = json["nodes"][i]["name"];
		}

		n.index = i;
		n.inverseBind = glm::mat4( 1.0 );
		n.meshIndex = -1;

		//First Set mesh
		if ( nodeJson.find( "mesh" ) != nodeJson.end() ) {
			n.meshIndex = nodeJson["mesh"]; // NEEDS index since a mesh can have multiple indices
		}
		//Set transformations
		if ( nodeJson.find( "translation" ) != nodeJson.end() ) {
			JSON posJ = nodeJson["translation"];
			glm::vec3 pos = glm::vec3( posJ[0], posJ[1], posJ[2] );
			n.t.SetPosition( pos );
		}

		if ( nodeJson.find( "scale" ) != nodeJson.end() ) {
			JSON posJ = nodeJson["scale"];
			glm::vec3 scale = glm::vec3( posJ[0], posJ[1], posJ[2] );
			n.t.SetScale( scale );
		}

		if ( nodeJson.find( "rotation" ) != nodeJson.end() ) {
			JSON posJ = nodeJson["rotation"];
			glm::quat rot = glm::quat( posJ[3], posJ[0], posJ[1], posJ[2] );
			glm::vec3 euler = glm::eulerAngles( rot );
			n.t.SetRotation( euler );
		}

		if ( nodeJson.find( "children" ) != nodeJson.end() ) {
			for ( int j = 0; j < nodeJson["children"].size(); j++ )
				n.children.push_back( &model.nodes[nodeJson["children"][j]] );
		}
		model.nodes[i] = n;
	}

	//==============//
	// GET SKIN		//
	//==============//
	if ( json.contains( "skins" ) ) {
		model.isStatic = false;
		if ( json["skins"].size() > 1 )
			std::cout << "MORE THAN 1 SKIN! WILL ONLY USE FIRST ONE " << std::endl;

		//Skin info
		Skin* skin = new Skin; // todo way to handle this, either remove on destructor or global place to hold all skins
		//memset( skin, 0, sizeof( Skin ) );
		for ( int i = 0; i < json["skins"][0]["joints"].size(); i++ ) {
			int nodeID = json["skins"][0]["joints"][i];
			skin->jointIDs.push_back( nodeID );
			model.nodes[nodeID].boneID = i;//set the node's boneID to the location in the bone array, this way i can use node->anim->nodeID == node->boneID
		}

		skin->name = json["skins"][0]["name"];

		//Inverse Bind
		int numInverseBind = json["skins"][0]["inverseBindMatrices"];


		JSON skinJson = json["skins"][0];
		int access = skinJson["inverseBindMatrices"];
		Accessor bindAccessor = GetAccessor( accessorJson[access] );


		//Todo replace with accessor and see if itw as broken or if imjust bad at debugging
		int length = json["bufferViews"][bindAccessor.bufferView]["byteLength"];
		int offset = json["bufferViews"][bindAccessor.bufferView]["byteOffset"];

		std::string data = binContents.substr( offset, length );
		skin->inverseBindMatrices.resize( bindAccessor.count );
		memcpy( skin->inverseBindMatrices.data(), data.c_str(), skin->inverseBindMatrices.size() * sizeof( glm::mat4 ) );

		model.skin = skin;

		//Give node* it's inverse bind matrix
		for ( int i = 0; i < skin->jointIDs.size(); i++ ) {
			int nodeID = model.nodes[skin->jointIDs[i]].index;// skin->jointIDs[i]
			model.nodes[nodeID].isJoint = true;
			model.nodes[nodeID].inverseBind = skin->inverseBindMatrices[i];
		}

	}

	//==============//
	//GET ANIMATIONS // 
	//==============//
	//model.animations.push_back(LoadAnimation(_path));
	LoadAnimations( _path );
	//==============//
	//GENERATE MESHES
	//==============//
	for ( int i = 0; i < json["meshes"].size(); i++ ) {
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec4> tangents;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::u16vec4> joints;
		std::vector<glm::vec4> weights;

		JSON meshJson = json["meshes"][i];


		//Used to get meshes for mesh* array inside node (at end of func.)
		int nodeID = -1;

		for ( int n = 0; n < model.nodes.size(); n++ ) {
			if ( model.nodes[n].meshIndex == i ) {
				nodeID = n;
			}
		}
		for ( int n = 0; n < meshJson["primitives"].size(); n++ ) {
			Mesh mesh;
			memset( &mesh, 0, sizeof( Mesh ) );
			JSON meshAttributeJson = meshJson["primitives"][n]["attributes"];

			//============= INDICES ==================

			JSON meshPrimJSON = meshJson["primitives"][n];


			if ( meshPrimJSON.find( "indices" ) != meshPrimJSON.end() ) {
				int accessorID = meshPrimJSON["indices"];

				Accessor accessor = GetAccessor( accessorJson[accessorID] );
				std::string data = GetBinDataFromAccessor( accessor, json["bufferViews"][accessor.bufferView], binContents );
				indices.resize( accessor.count );
				memcpy( indices.data(), data.c_str(), data.size() );
			}


			// ================= ADD TEXTURES ================== //
			if ( meshPrimJSON.find( "material" ) != meshPrimJSON.end() ) {
				int materialID = meshPrimJSON["material"];
				JSON matJson = json["materials"][materialID];
				Material& m = materials[materialID];

				if ( m.baseColorTexture != -1 ) {
					std::string texPath = directory;
					int texID = json["textures"][m.baseColorTexture]["source"];
					texPath += json["images"][texID]["uri"];
					mesh.diffuseTexture = ResourceManager::Get().GetTexture( texPath.c_str() );
				}
				if ( m.normalTexture != -1 ) {
					std::string texPath = directory;
					int texID = json["textures"][m.normalTexture]["source"];
					texPath += json["images"][texID]["uri"];
					mesh.normalTexture = ResourceManager::Get().GetTexture( texPath.c_str() );
				}
				if ( m.normalTexture != -1 ) {
					std::string texPath = directory;
					int texID = json["textures"][m.metallicRoughnessTexture]["source"];
					texPath += json["images"][texID]["uri"];
					mesh.specularTexture = ResourceManager::Get().GetTexture( texPath.c_str() );
				}
			}

			// ===== POSITIONS =========== /

			if ( meshAttributeJson.find( "POSITION" ) != meshAttributeJson.end() ) {
				int accessorID = meshAttributeJson["POSITION"];

				Accessor accessor = GetAccessor( accessorJson[accessorID] );
				positions.resize( accessor.count );

				std::string data = GetBinDataFromAccessor( accessor, json["bufferViews"][accessor.bufferView], binContents );
				memcpy( positions.data(), data.c_str(), data.size() );

			}

			//Scale down for now

			// ========== NORMALS =========== //

			if ( meshAttributeJson.find( "NORMAL" ) != meshAttributeJson.end() ) {
				int accessorID = meshAttributeJson["NORMAL"];

				Accessor accessor = GetAccessor( accessorJson[accessorID] );
				normals.resize( accessor.count );

				std::string data = GetBinDataFromAccessor( accessor, json["bufferViews"][accessor.bufferView], binContents );
				memcpy( normals.data(), data.c_str(), data.size() );



			}

			// ===== TANGENTS =========== /
			if ( meshAttributeJson.find( "TANGENT" ) != meshAttributeJson.end() ) {
				//This gets converted to vec3, only reason for W component is the sign to multiply by for computing bitangent incase the direction is flipped, theretically should never need this
				int accessorID = meshAttributeJson["TANGENT"];

				Accessor accessor = GetAccessor( accessorJson[accessorID] );
				tangents.resize( accessor.count );

				std::string data = GetBinDataFromAccessor( accessor, json["bufferViews"][accessor.bufferView], binContents );
				memcpy( tangents.data(), data.c_str(), data.size() );

			}

			// ========== TEX COORDS =========== //

			if ( meshAttributeJson.find( "TEXCOORD_0" ) != meshAttributeJson.end() ) {
				int accessorID = meshAttributeJson["TEXCOORD_0"];

				Accessor accessor = GetAccessor( accessorJson[accessorID] );
				texCoords.resize( accessor.count );

				std::string data = GetBinDataFromAccessor( accessor, json["bufferViews"][accessor.bufferView], binContents );
				memcpy( texCoords.data(), data.c_str(), data.size() );
			}


			// ========== JOINTS	=========== //
			if ( meshAttributeJson.find( "JOINTS_0" ) != meshAttributeJson.end() ) {
				int accessorID = meshAttributeJson["JOINTS_0"];

				Accessor accessor = GetAccessor( accessorJson[accessorID] );
				joints.resize( accessor.count );

				std::string data = GetBinDataFromAccessor( accessor, json["bufferViews"][accessor.bufferView], binContents );
				//first cast to unsigned short
				memcpy( joints.data(), data.c_str(), data.size() );
			}


			// ==========  WEIGHTS   =========== //
			if ( meshAttributeJson.find( "WEIGHTS_0" ) != meshAttributeJson.end() ) {
				int accessorID = meshAttributeJson["WEIGHTS_0"];

				Accessor accessor = GetAccessor( accessorJson[accessorID] );
				weights.resize( accessor.count );

				std::string data = GetBinDataFromAccessor( accessor, json["bufferViews"][accessor.bufferView], binContents );
				//first cast to unsigned short
				memcpy( weights.data(), data.c_str(), data.size() );
			}



			vertices.resize( positions.size() );
			memset( vertices.data(), 0, vertices.size() * sizeof( Vertex ) );

			for ( int i = 0; i < vertices.size(); i++ ) {
				vertices[i].pos = positions[i];

				if ( texCoords.size() > 0 )
					vertices[i].texCoords = texCoords[i];

				if ( normals.size() > 0 )
					vertices[i].normal = normals[i];

				//TODO check and make sure sponza looks the same without the W component
				if ( tangents.size() > 0 ) {
					vertices[i].tangent = glm::vec3( tangents[i].x, tangents[i].y, tangents[i].z );
				}

				//cast from ushort to uint ( probably maybe revert back to ushort )
				if ( joints.size() > 0 ) {
					vertices[i].boneIDs[0] = ( int ) joints[i].x;
					vertices[i].boneIDs[1] = ( int ) joints[i].y;
					vertices[i].boneIDs[2] = ( int ) joints[i].z;
					vertices[i].boneIDs[3] = ( int ) joints[i].w; 


				}

				if ( weights.size() > 0 ) {
					vertices[i].weights = weights[i];
				}
			}

			mesh.SetupBuffers( vertices, indices );
			model.meshes.push_back( mesh );

			//Add mesh that was just created
			model.nodes[nodeID].meshIndices.push_back( model.meshes.size() - 1 );
		}
	}

	if ( model.skin != nullptr ) {
		//BuildBoneHeirechy(model, &model.nodes[model.rootNode], nullptr);
	}

	return model;
}

void LoadAnimations( const char* _path ) {
	std::string path = _path;
	std::string fileContent = FileContentToString( path );
	JSON json = JSON::parse( fileContent );

	std::string directory = path.substr( 0, path.find_last_of( "/" ) + 1 );
	std::string binPath = directory;
	binPath += json["buffers"][0]["uri"];
	unsigned int binSize = json["buffers"][0]["byteLength"];
	std::string binContents = FileContentToString( binPath );


	if ( json.find( "animations" ) != json.end() ) {
		for ( int i = 0; i < json["animations"].size(); i++ ) {
			//Animation animation;
			std::string name = "no name";

			if ( json["animations"][i].find( "name" ) != json["animations"][i].end() )
				name = json["animations"][i]["name"];
			
			//Skip over animation if it already exists inside of the manager (NOTE: could be overlap with differnet models withs ame name)
			if ( ResourceManager::Get().ContainsAnimation( name ) )
				continue;

			ResourceManager::Get().CreateAnimation( name );
			Animation& animation = *ResourceManager::Get().GetAnimation( name );
			animation.name = name;

			for ( int n = 0; n < json["animations"][i]["channels"].size(); n++ ) {
				JSON channelJson = json["animations"][i]["channels"][n];

				AnimChannel* channel = nullptr;

				//First check if a channel already exists with this index, that way it's easier to keep track of
				for ( int a = 0; a < animation.animChannels.size(); a++ ) {
					if ( animation.animChannels[a].nodeID == channelJson["target"]["node"] ) {
						channel = &animation.animChannels[a];
						break;
					}
				}

				//did not find a valid one
				if ( channel == nullptr ) {
					AnimChannel a;
					animation.animChannels.push_back( a );
					channel = &animation.animChannels[animation.animChannels.size() - 1];
				}

				//AnimChannel channel{ 0 };
				channel->nodeID = channelJson["target"]["node"];

				int sampler = channelJson["sampler"];
				JSON samplerJson = json["animations"][i]["samplers"][sampler];

				int inputIndex = samplerJson["input"];
				Accessor inputAccessor = GetAccessor( json["accessors"][inputIndex] );
				std::string inputBuffer = GetBinDataFromAccessor( inputAccessor, json["bufferViews"][inputAccessor.bufferView], binContents );

				int outputIndex = samplerJson["output"];
				Accessor outputAccessor = GetAccessor( json["accessors"][outputIndex] );
				std::string outputBuffer = GetBinDataFromAccessor( outputAccessor, json["bufferViews"][outputAccessor.bufferView], binContents );

				if ( channelJson["target"]["path"] == "translation" ) {
					std::vector<float> times( inputAccessor.count );
					memcpy( times.data(), inputBuffer.data(), times.size() * sizeof( float ) );
					std::vector<glm::vec3> translations( outputAccessor.count );
					memcpy( translations.data(), outputBuffer.data(), translations.size() * sizeof( glm::vec3 ) );

					channel->translations.resize( inputAccessor.count );
					for ( int t = 0; t < channel->translations.size(); t++ ) {
						channel->translations[t].time = times[t];
						channel->translations[t].translation = translations[t];
					}
				}
				else if ( channelJson["target"]["path"] == "scale" ) {
					std::vector<float> times( inputAccessor.count );
					memcpy( times.data(), inputBuffer.data(), times.size() * sizeof( float ) );
					std::vector<glm::vec3> scales( outputAccessor.count );
					memcpy( scales.data(), outputBuffer.data(), scales.size() * sizeof( glm::vec3 ) );

					channel->scales.resize( inputAccessor.count );
					for ( int t = 0; t < channel->translations.size(); t++ ) {
						channel->scales[t].time = times[t];
						channel->scales[t].scale = scales[t];
					}
				}
				else if ( channelJson["target"]["path"] == "rotation" ) {
					std::vector<float> times( inputAccessor.count );
					memcpy( times.data(), inputBuffer.data(), times.size() * sizeof( float ) );
					std::vector<glm::vec4> rotations( outputAccessor.count );
					memcpy( rotations.data(), outputBuffer.data(), rotations.size() * sizeof( glm::quat ) );

					channel->rotations.resize( inputAccessor.count );
					for ( int t = 0; t < channel->rotations.size(); t++ ) {
						channel->rotations[t].time = times[t];
						channel->rotations[t].rotaiton = glm::quat( rotations[t].w, rotations[t].x, rotations[t].y, rotations[t].z );
					}
				}
			}
		}
	}
}

Texture LoadTexture( const char* path, TextureType type ) {

	Texture texture;
	texture.tType = type;

	stbi_set_flip_vertically_on_load( 0 );
	unsigned char* data;
	int width;
	int height;
	int channels;

	data = stbi_load( path, &width, &height, &channels, 0 );

	if ( !data ) {
		std::cout << "[ERROR] COULD NOT LOAD TEXTURE AT: " << path << std::endl;
	}

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glGenTextures( 1, &texture.textureID );
	glBindTexture( GL_TEXTURE_2D, texture.textureID );

	//unsigned int imageType = ( channels == 3 ) ? GL_RGB : GL_RGBA;
	switch ( channels ) {
	case ( 3 ):
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
		break;
	case ( 4 ):
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
		break;
	case ( 1 ):
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
		break;
	default:
		std::cout << "there are " << channels << " channels in " << path << std::endl;
	}

	stbi_image_free( data );
	return texture;
}


std::string FileContentToString( std::string path ) {
	std::ifstream in( path, std::ios::binary );
	if ( in ) {
		std::string contents;
		in.seekg( 0, std::ios::end );
		contents.resize( in.tellg() );
		in.seekg( 0, std::ios::beg );
		in.read( &contents[0], contents.size() );
		in.close();
		return contents;
	}
	else
		std::cout << "error reading " << path << std::endl;
	return std::string( "ERROR" );
}