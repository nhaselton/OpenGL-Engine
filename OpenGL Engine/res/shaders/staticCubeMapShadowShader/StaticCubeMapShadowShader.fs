#version 330 core

uniform vec3 lightPos;
out float LightToVertex;
in vec3 vWorld;

void main()
{             
    vec3 lightToVertex = vWorld - lightPos;
    LightToVertex = length(LightToVertex);
    //gl_FragDepth = gl_FragCoord.z;
}  
