#version 330 core

uniform vec3 lightPos;
uniform float far_plane;
in vec3 vFragPos;

void main()
{
    float lightDistance = length(vFragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}