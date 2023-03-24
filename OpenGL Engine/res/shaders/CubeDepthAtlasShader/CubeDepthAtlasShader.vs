#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
out vec3 vFragPos;

void main()
{   
    vFragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}