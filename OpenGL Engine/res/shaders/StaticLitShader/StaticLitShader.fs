#version 330 core
out vec4 FragColor;

struct Light {
  	vec3 color;
	vec3 direction;
	vec3 pos;
	float linear;
	float quadratic;
	float cutoff;
	float outerCutoff;
};

uniform sampler2D albedo;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

uniform Light pointLights[32];
uniform Light spotLights[32];
uniform Light directionalLight;

uniform int	numPointLights;
uniform int	numSpotLights;

uniform float maxBias;

uniform vec3 viewPos;


in vec4 directionalLightLightFragPos;

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;
in mat3 TBN;
//in vec4 vFragLightSpace;

uniform bool showNormalMap;
uniform bool showSpecularMap;

vec3 CalcDirectional(vec3 normal, float specular);
vec3 CalcPointLights(Light light, vec3 normal ,float specular);
vec3 CalcSpotLights(Light light, vec3 normal ,float specular);
float CalcShadow(vec4 fragLightSpace, vec3 normal, vec3 lightDir);

void main(){
	vec3 normal;
	float specular;

	if ( showNormalMap ){
		normal = texture(normalMap, vTexCoords).rgb;
		normal = normal * 2.0 - 1.0;   
		normal = normalize(TBN * normal); 
	}else {
		normal = vNormal;	
	}

	if ( showSpecularMap ){
		specular = texture(specularMap, vTexCoords).b;
	}else{
		specular = 1;
	}

	vec3 color = texture(albedo,vTexCoords).rgb;

	vec3 result = vec3(0);
	//Setup) 
	//	1. single directional 
	//	2. point lights, 
	//	3. spotLights

	//Directional Light
	result += CalcDirectional( normal,specular );

	//Point Lights
	//for ( int i =0 ; i < numPointLights; i++ )
	//	result += CalcPointLights(pointLights[i], normal, specular );

	//Spot Lights
	//for ( int i =0; i < numSpotLights; i++ )
	//	result += CalcSpotLights(spotLights[i], normal, specular );

	result += .1; //Ambient
	result *= color;
	FragColor = vec4(result,1.0);

}
float CalcShadow(vec4 fragLightSpace, vec3 normal, vec3 lightDir){
	vec3 projCoords = fragLightSpace.xyz / fragLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	
	
	float minBias = maxBias/10;
	float bias = max(.05 * (1.0 - dot(normal, lightDir)), .005);  
	//float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias);  
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	return shadow;
}


vec3 CalcDirectional(vec3 normal ,float specular){
	vec3 result = vec3(0);
	vec3 norm = normalize(normal);
    //vec3 lightDir = normalize(-directionalLight.direction);
    vec3 lightDir = normalize(directionalLight.pos - vFragPos);
	float diffuse = max(dot(normal, lightDir), 0.0);

    // == Specular == // 
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * specular;

	float shadow = CalcShadow(directionalLightLightFragPos, normal, lightDir);
	
	//diffuse =1;
	result = (1.0 - shadow) * (diffuse + spec) * directionalLight.color;
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