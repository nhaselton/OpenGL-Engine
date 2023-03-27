#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in vec4 aWeights;
layout (location = 6) in ivec4 aBoneIDs;

uniform mat4 model;

uniform mat4[100] bones;
uniform mat4 lightSpaceMatrix;
out vec3 vFragPos;
void main() {
    mat4 skinMat =     aWeights.x * bones[aBoneIDs.x] +
                    aWeights.y * bones[aBoneIDs.y] +
                    aWeights.z * bones[aBoneIDs.z] + 
                    aWeights.w * bones[aBoneIDs.w];
 
    vFragPos = vec3(model * ( skinMat * vec4(aPos, 1.0)));
    
    gl_Position = lightSpaceMatrix * model * (skinMat * vec4(aPos.x, aPos.y, aPos.z, 1.0));


}