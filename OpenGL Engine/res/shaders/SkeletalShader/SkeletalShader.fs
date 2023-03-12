#version 330 core
out vec4 FragColor;

uniform sampler2D albedo;

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

in vec4 vWeights;
flat in vec4 vBoneIDs;

void main()
{
    if ( vBoneIDs.x ==1){
        FragColor = vec4(1);
    }else
    FragColor = vec4(0,0,0,1);
    
}