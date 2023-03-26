#version 330 core
out vec4 FragColor;

uniform sampler2D albedo;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

in vec2 vTexCoords;

uniform vec3 lightColor;
uniform vec3 lightDirection;

in vec3 vViewPos;
in vec3 vFragPos;
in vec3 vLightPos;
in vec3 vNormal;

uniform float lightQuadratic;
uniform float lightLinear;


void main()
{
    vec3 color = texture(albedo,vTexCoords).rgb;//vec4(1.0f, 0.5f, 0.2f, 1.0f);
 
    // == AMBIENT == // 
    float ambient  = 0.1;

    // == DIFFUSE == // 
    vec3 norm;
    norm = texture(normalMap,vTexCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);

    vec3 lightDir = normalize( vLightPos - vFragPos );
    float diffuse = max(dot(norm, lightDir), 0.0);

    // == Specular == // 
    vec3 viewDir = normalize(vViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float specImage = texture(specularMap,vTexCoords).r;
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * specImage;
    
    float fragDist = distance(vLightPos, vFragPos );

    float attenuation = 1.0 / ( 1.0 + lightLinear * fragDist + lightQuadratic * ( fragDist * fragDist ) );

    vec3 result = (ambient + diffuse + specular) * color * attenuation * lightColor;
    
    FragColor = vec4(result,1.0);
}