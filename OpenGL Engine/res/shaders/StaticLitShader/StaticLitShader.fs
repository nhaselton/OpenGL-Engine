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
	float farPlane;
	int	  ID;
};

//Samplers
uniform sampler2D albedo;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;
uniform samplerCube cubeMap;

//Lights

uniform Light pointLights[32];
uniform Light spotLights[32];
uniform Light directionalLight;

uniform mat4 directionalLightSpaceMatrix;
uniform mat4[32] spotLightSpaceMatrices;

uniform int numPointLights;
uniform int numSpotLights;

//Others
uniform vec3 viewPos;
uniform bool showNormalMap;
uniform bool showSpecularMap;



in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;
in vec3 vWorldSpace;
in mat3 TBN;
//in vec4 vFragLightSpace;


vec3 CalcDirectional(vec3 normal, float specular);
vec3 CalcPointLights(Light light, vec3 normal ,float specular);
vec3 CalcSpotLights(Light light, vec3 normal ,float specular,int lightID);
float CalcShadow(vec4 fragLightSpace, vec3 normal, vec3 lightDir);
vec3 ShadowOffsetLookUp(sampler2D map, vec4 loc, vec2 offset);
float CalcCubeShadow(Light light, vec3 fragPos);


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
	for ( int i =0 ; i < numPointLights; i++ )
		result += CalcPointLights(pointLights[i], normal, specular );

	//Spot Lights
	for ( int i =0; i < numSpotLights; i++ )
		result += CalcSpotLights(spotLights[i], normal, specular, i );
	result += vec3(.05f);//ambient
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
	
	//out of map is shadow
	if( projCoords.z > 1.0)
		return 1;

	// == Brute Force Shadow Sampler == //
	float bias = .05;
	float sampleShadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	
	for(int x = -1; x <= 1; x++) {
		for(int y = -1; y <= 1; y++) {
			float depth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			sampleShadow += currentDepth - bias > depth ? 1 : 0;
		}
	}
	sampleShadow /= 9.0;
	
	return sampleShadow;
}


float CalcCubeShadow(Light light, vec3 fragPos){
	vec3 fragToLight = vFragPos - light.pos; 
    float closestDepth = texture(cubeMap, fragToLight).r;
	closestDepth *= light.farPlane;  
	float currentDepth = length(fragToLight);  
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;     

	// == Brute Force Shadow Sampler == //
	//float shadow  = 0.0;
	float bias    = 0.05; 
	float samples = 4.0;
	float offset  = 0.1;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				float closestDepth = texture(cubeMap, fragToLight + vec3(x, y, z)).r; 
				closestDepth *= light.farPlane;   // undo mapping [0;1]
				if(currentDepth - bias > closestDepth)
					shadow += 1.0;
			}
		}
	}
	shadow /= (samples * samples * samples);

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

	vec4 directionalLightLightFragPos = directionalLightSpaceMatrix * vec4(vFragPos, 1.0);
	float shadow = CalcShadow(directionalLightLightFragPos, normal, -lightDir);

	diffuse = 1;

	result = (1.0 - shadow) * diffuse *   directionalLight.color;
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
    
	diffuse *= attenuation;
	specular *= attenuation;

	float shadow = CalcCubeShadow(light,vFragPos);

    vec3 result = (1.0 - shadow) * vec3(diffuse + spec ) * light.color;
	return result;
}

vec3 CalcSpotLights(Light light, vec3 normal ,float specular, int lightID) {

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
	attenuation = 1.0;
    vec3 result = vec3(0);
    
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
	
	// -- Shadow -- //
	vec4 lightFragPos = spotLightSpaceMatrices[lightID] * vec4(vFragPos, 1.0);

	float shadow = CalcShadow(lightFragPos , normal, lightDir);


	
	result = (1.0-shadow) * (diffuse + specular) * light.color; 
	return result;
}