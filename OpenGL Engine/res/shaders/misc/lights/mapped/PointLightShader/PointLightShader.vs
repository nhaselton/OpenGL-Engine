#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec2 vTexCoords;

out mat3 TBN;


uniform vec3 viewPos;
uniform vec3 lightPos;

out vec3 vViewPos;
out vec3 vFragPos;
out vec3 vLightPos;
out vec3 vNormal;

void main() {
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = transpose(mat3(T, B, N));
    
    vViewPos =  TBN * viewPos;
    vFragPos =  TBN * vec3(model * vec4(aPos, 1.0));
    vLightPos = TBN * lightPos;
      
    vTexCoords = aTexCoords;
    
    vNormal = aNormal;


    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}