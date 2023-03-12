#version 330 core
out vec4 FragColor;

uniform sampler2D albedo;
uniform sampler2D normalMap;

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;

in mat3 TBN;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Light {
  	vec3 color;
	vec3 direction;
	vec3 pos;
	float linear;
	float quadratic;
	float cutoff;
	float outerCutoff;
};

uniform Light pointLights[32];
uniform Light spotLights[32];
uniform Light directionalLight;

uniform int	numPointLights;
uniform int	numSpotLights;

uniform bool showNormalMap;
uniform bool showSpecularMap;

vec3 CalcDirectional(vec3 normal, float specular);
vec3 CalcPointLights(Light light, vec3 normal ,float specular);
vec3 CalcSpotLights(Light light, vec3 normal ,float specular);

void main()
{
		vec3 color = texture(albedo, vTexCoords).rgb;

		vec3 result = vec3(0);
		//Setup) 
		//	1. single directional 
		//	2. point lights, 
		//	3. spotLights
	
		vec3 normal = vNormal;
		if ( showNormalMap ){
			normal = texture(normalMap, vTexCoords).rgb;
			normal = normal * 2.0 - 1.0;   
			normal = normalize(TBN * normal); 
		}
		//Directional Light
		float specular = 1;
		
		result += CalcDirectional( normal,specular );
	
		//Point Lights
		for ( int i =0 ; i < numPointLights; i++ )
			result += CalcPointLights(pointLights[i], normal, specular );
	
		//Spot Lights
		for ( int i =0; i < numSpotLights; i++ )
			result += CalcSpotLights(spotLights[i], normal, specular );
	
		
	
		result *= color;
		FragColor = vec4(result,1.0);
}

vec3 CalcDirectional(vec3 normal ,float specular){
	vec3 result = vec3(0);
	vec3 norm = normalize(normal);
    vec3 lightDir = normalize(directionalLight.direction);
    float diffuse = max(dot(normal, lightDir), 0.0);

    // == Specular == // 
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * specular;

	result = (diffuse + spec) * directionalLight.color;
	return result;
}

vec3 CalcPointLights(Light light ,vec3 normal ,float specular){
	vec3 lightDir = normalize( light.pos - vFragPos );
    float diffuse = max(dot(normal, lightDir), 0.0);

    // == Specular == // 
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * specular;
    float fragDist = distance(light.pos, vFragPos );

    float attenuation = 1.0 / ( 1.0 + light.linear * fragDist + light.quadratic * ( fragDist * fragDist ) );
    
    vec3 result = vec3(diffuse + spec ) * light.color;
	return result;
}

vec3 CalcSpotLights(Light light, vec3 normal ,float specular) {

    vec3 lightDir = normalize( light.pos - vFragPos );
    // == DIFFUSE == // 
    float diffuse = max(dot(normal, lightDir), 0.0);

    // == Specular == // 
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * specular;

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon   = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);    
   
    float fragDist = length(light.pos - vFragPos );
    float attenuation = 1.0 / ( 1.0 + light.linear * fragDist + light.quadratic * ( fragDist * fragDist ) );

    vec3 result = vec3(0);
    
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    result = (diffuse + specular) * light.color; 
	return result;
}