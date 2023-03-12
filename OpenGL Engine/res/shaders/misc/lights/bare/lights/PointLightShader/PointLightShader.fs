#version 330 core
out vec4 FragColor;

uniform sampler2D albedo;

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float lightQuadratic;
uniform float lightLinear;


void main()
{
    vec3 color = texture(albedo,vTexCoords).rgb;//vec4(1.0f, 0.5f, 0.2f, 1.0f);
 
    // == AMBIENT == // 
    float ambient  = 0.1;

    // == DIFFUSE == // 
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize( lightPos - vFragPos );
    float diffuse = max(dot(vNormal, lightDir), 0.0);

    // == Specular == // 
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    
    float fragDist = distance(lightPos, vFragPos );

    float attenuation = 1.0 / ( 1.0 + lightLinear * fragDist + lightQuadratic * ( fragDist * fragDist ) );

    
    vec3 result = (ambient + diffuse + specular) * color * lightColor * attenuation;

    FragColor = vec4(result ,1);
}