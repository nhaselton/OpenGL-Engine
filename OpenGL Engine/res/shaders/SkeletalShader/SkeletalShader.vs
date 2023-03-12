#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 5) in vec4 aWeights;
layout (location = 6) in ivec4 aBoneIDs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vTexCoords;
out vec3 vFragPos;
out vec3 vNormal;

out vec4 vWeights;
flat out vec4 vBoneIDs;


uniform mat4[100] bones;

void main() {
        vWeights = aWeights;
        vBoneIDs = aBoneIDs;
        vTexCoords = aTexCoords;
        vFragPos = vec3(model * vec4(aPos, 1.0));
        vNormal = aNormal;

        mat4 skinMat =  aWeights.x * bones[aBoneIDs.x] +
                        aWeights.y * bones[aBoneIDs.y] +
                        aWeights.z * bones[aBoneIDs.z] + 
                        aWeights.w * bones[aBoneIDs.w];
        
    gl_Position = projection * view * model * ( skinMat * vec4(aPos.x, aPos.y, aPos.z, 1.0));
}