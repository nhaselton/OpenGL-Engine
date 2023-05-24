#include "Mesh.h"
#include <iostream>
#include "Vertex.h"
#include <glm/gtx/string_cast.hpp>

Mesh::Mesh() {
	diffuseTexture = nullptr;
	normalTexture  = nullptr;
	specularTexture = nullptr;
}

Mesh::Mesh( float* vertices, int numVerts, unsigned int* indices, int numIndices ) {
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, numVerts * sizeof( float ), vertices, GL_STATIC_DRAW );

	glGenBuffers( 1, &EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof( unsigned int ), indices, GL_STATIC_DRAW );

	this->numVertices = numVerts;
	this->numIndices = numIndices;

	BindAttribPointer( 0, 3, GL_FLOAT, 3 * sizeof( float ), ( void* ) 0 );
}

Mesh::Mesh( std::vector<Vertex> vertices ) {
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), vertices.data(), GL_STATIC_DRAW );

	numIndices = 0;//indices.size();
	numVertices = vertices.size();

	glBindVertexArray( VAO );
	BindAttribPointer( 0, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) 0 );
	BindAttribPointer( 1, 2, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, texCoords ) );
	BindAttribPointer( 2, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, normal ) );
	BindAttribPointer( 3, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, tangent ) );
	BindAttribPointer( 4, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, biTangent ) );

		
}

Mesh::Mesh( std::vector<Vertex> vertices, std::vector<unsigned short> indices ) {
	numVertices = vertices.size();
	numIndices = indices.size();
	
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), vertices.data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size()  * sizeof( unsigned short ), indices.data(), GL_STATIC_DRAW );
	

	BindAttribPointer( 0, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) 0 );
	BindAttribPointer( 1, 2, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, texCoords ) );
	BindAttribPointer( 2, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, normal ) );
}

Mesh::Mesh( std::vector<glm::vec3> positions, std::vector<unsigned short> indices ) {
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, positions.size() * sizeof( glm::vec3 ), positions.data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size()  * sizeof( unsigned short ), indices.data(), GL_STATIC_DRAW );

	 
	numIndices = indices.size();
	numVertices = positions.size();

	BindAttribPointer( 0, 3, GL_FLOAT, 3 * sizeof( float ), ( void* ) 0 );
}

void Mesh::BindAttribPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer ) {
	glVertexAttribPointer( index, size, type, GL_FALSE, stride, pointer );
	glEnableVertexAttribArray( index );
}


void Mesh::SetupBuffers( std::vector<Vertex> vertices, std::vector<unsigned short> indices ) {
	numVertices = vertices.size();
	numIndices = indices.size();

	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), vertices.data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned short ), indices.data(), GL_STATIC_DRAW );


	BindAttribPointer( 0, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) 0 );
	BindAttribPointer( 1, 2, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, texCoords ) );
	BindAttribPointer( 2, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, normal ) );
	BindAttribPointer( 3, 3, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, tangent ) );

	BindAttribPointer( 5, 4, GL_FLOAT, sizeof( Vertex ), ( void* ) offsetof( Vertex, weights ) );
	//BindAttribPointer( 6, 4, GL_INT , sizeof( Vertex ), ( void* ) offsetof( Vertex, boneIDs ) );
	glVertexAttribIPointer( 6, 4, GL_INT, sizeof( Vertex ), (void*) offsetof( Vertex, boneIDs ) );
	glEnableVertexAttribArray( 6 );

}

void Mesh::BindVBO() {
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
}

void Mesh::BindVAO() {
	glBindVertexArray( VAO );
}

void Mesh::BindEBO() {
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
}