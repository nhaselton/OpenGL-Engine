#pragma once
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Vertex.h"
#include "Texture.h"

class Mesh {
public:
	Mesh();
	Mesh( float* vertices, int numVerts, unsigned int* indices, int numIndices );
	Mesh( std::vector<Vertex> vertices );
	Mesh( std::vector<Vertex> positions, std::vector<unsigned short> indices );
	
	unsigned int	VBO;
	unsigned int	VAO;
	unsigned int	EBO;

	unsigned int	numVertices;
	unsigned int	numIndices;

	Texture*		diffuseTexture;
	Texture*		normalTexture;
	Texture*		specularTexture;

	void			SetupBuffers( std::vector<Vertex> positions, std::vector<unsigned short> indices );
	void			BindAttribPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer );
	void			BindVBO();
	void			BindVAO();
	void			BindEBO();

	int				id; //todo remove ( debug )

private:
	Mesh( std::vector<glm::vec3> positions, std::vector<unsigned short> indices );
};