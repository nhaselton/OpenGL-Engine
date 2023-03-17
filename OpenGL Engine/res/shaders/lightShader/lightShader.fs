#version 330 core
out vec4 FragColor;

in vec3 lightColor;


uniform vec3 color;

void main()
{
    FragColor = vec4(0,1,0,1);
}