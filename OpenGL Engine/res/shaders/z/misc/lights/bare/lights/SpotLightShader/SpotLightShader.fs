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
uniform float cutoff;
uniform float outerCutoff;

void main()
{
    vec3 color = texture(albedo,vTexCoords).rgb;//vec4(1.0f, 0.5f, 0.2f, 1.0f);
    vec3 lightDir = normalize( lightPos - vFragPos );


    // == AMBIENT == // 
    float ambient  = 0.1;
    float specular = 0;
    float diffuse  = 0;

    // == DIFFUSE == // 
    vec3 norm = normalize(vNormal);
    diffuse = max(dot(vNormal, lightDir), 0.0);

    // == Specular == // 
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);


    float theta = dot(lightDir, normalize(-lightDirection)); 
    float epsilon   = cutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);    
   
    
    float fragDist = length(lightPos - vFragPos );
    float attenuation = 1.0 / ( 1.0 + lightLinear * fragDist + lightQuadratic * ( fragDist * fragDist ) );

    vec3 result = vec3(0);
    
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    result = (ambient + diffuse + specular) * color; 
    //result = ambient + ((diffuse + specular)  * attenuation * intensity * lightColor) * color;

    FragColor = vec4(result ,1);
}