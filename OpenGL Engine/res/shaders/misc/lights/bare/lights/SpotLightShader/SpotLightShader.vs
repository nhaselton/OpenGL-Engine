#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vTexCoords;
out vec3 vFragPos;
out vec3 vNormal;

void main() {
    vTexCoords = aTexCoords;
    vFragPos = vec3(model * vec4(aPos, 1.0));
    vNormal = aNormal;
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}