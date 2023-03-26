#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 5) in vec4 aWeights;
layout (location = 6) in ivec4 aBoneIDs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4[100] bones;

out vec2 vTexCoords;
out vec3 vFragPos;
out vec3 vNormal;
out mat3 TBN;

void main() {
    vTexCoords = aTexCoords;
    vNormal = aNormal;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    mat4 skinMat =  aWeights.x * bones[aBoneIDs.x] +
                    aWeights.y * bones[aBoneIDs.y] +
                    aWeights.z * bones[aBoneIDs.z] + 
                    aWeights.w * bones[aBoneIDs.w];
       
    vFragPos = vec3(model * ( skinMat * vec4(aPos, 1.0)));
    gl_Position = projection * view * model * ( skinMat * vec4(aPos.x, aPos.y, aPos.z, 1.0));
}