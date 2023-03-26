#version 330 core
out vec4 FragColor;

struct Light {
  	vec3  color;
	vec3  direction;
	vec3  pos;
	float linear;
	float quadratic;
	float cutoff;
	float outerCutoff;
	float farPlane;
	int	  ID;
	vec4  shadowUVs;
};

//Samplers
uniform sampler2D albedo;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D shadowAtlas;


//Lights

uniform Light pointLights[32];
uniform Light spotLights[32];
uniform Light directionalLight;

uniform mat4 directionalLightSpaceMatrix;
uniform mat4 spotLightSpaceMatrices[32];

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
float CalcShadow(vec4 fragLightSpace, Light light);
vec3 ShadowOffsetLookUp(sampler2D map, vec4 loc, vec2 offset);
float CalcPointLightShadows(Light light);


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

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}


float CalcPointLightShadows(Light light){
	vec3 fragToLight = vFragPos - light.pos; 
	// helpers to figure out 
	float x = fragToLight.x;
    float y = fragToLight.y;
    float z = fragToLight.z;

    float absX = abs( x );
    float absY = abs( y );
    float absZ = abs( z );

    int index;
    float maxAxis, uc, vc;

	float u = 0, v = 0;

    bool isXPositive = x > 0 ? true  : false ;
    bool isYPositive = y > 0 ? true  : false ;
    bool isZPositive = z > 0 ? true  : false ;

	// Figure out the face of the cube you are on, then get the UVs of it,
	// These change depending on the face directions

	//+X
	vec2 offset = vec2(0);
	if (isXPositive && absX >= absY && absX >= absZ) {
		index = 0;
	    maxAxis = absX;
		uc = -z;
		vc = y;
		offset = vec2(1,0);
	}
	//-X
	if (!isXPositive  && absX >= absY && absX >= absZ) {
		index = 1;
		maxAxis = absX;
		uc = z;
		vc = y;
		offset = vec2(-1,0);
	}
	 //+Y
	if (isYPositive && absY >= absX && absY >= absZ) {
		index = 2;
		maxAxis = absY;
		uc = x;
	    vc = -z;
		offset = vec2(0,-1);
	}
	//-Y
	if (!isYPositive && absY >= absX && absY >= absZ) {
		index = 3;
		maxAxis = absY;
	    uc = x;
	    vc = z;
		offset = vec2(0,1);
	}
	//+Z
	if (isZPositive  && absZ >= absX && absZ >= absY) {
		index = 4;
		maxAxis = absZ;
	    uc = x;
	    vc = y;
		offset = vec2(0);
	}
	//-Z
	if (!isZPositive && absZ >= absX && absZ >= absY) {
		index = 5;
		maxAxis = absZ;
	    uc = -x;
	    vc = y;
		offset = vec2(2,0);
	}
	vc = -vc;

	//convert UVs from -1 to 0 to 1
	u = 0.5f * (uc / maxAxis + 1.0f);
	v = 0.5f * (vc / maxAxis + 1.0f);
		//How cubemap is setup
	//	  +Y
	// -X +Z +X -Z
	//    -Y
	//atlas UV = end

	//get start and end of X&Ys
	vec4 atlas = light.shadowUVs;
	
	float xOffset = offset.x * ( 1024.0 / 8192.0 );
	float yOffset = offset.y * ( 1024.0 / 8192.0 );

	atlas.x += xOffset;
	atlas.z += xOffset;
	atlas.y += yOffset;
	atlas.w += yOffset;
	
	u = map ( u, 0.0 , 1.0 , atlas.x, atlas.z);
	v = map ( v, 0.0 , 1.0 , atlas.y, atlas.w);
    float closestDepth = texture(shadowAtlas, vec2(u,v)).r;
	closestDepth *= light.farPlane;  
	float currentDepth = length(fragToLight);  
	float shadow = currentDepth - .05 > closestDepth ? 1.0 : 0.0;    
		// == Brute Force Shadow Sampler == //

	// == Brute Force Shadow Sampler == //
	float bias = .05;
	float sampleShadow = 0.0;
	//seems to work with texture atlas size not defualt map size, wonder if its suppose to do that
	vec2 texelSize = vec2(1.0 / 8096);//textureSize(shadowMap, 0);

	for(int x = -1; x <= 1; x++) {
		for(int y = -1; y <= 1; y++) {
			float depth = texture(shadowAtlas, vec2(u,v) + vec2(x,y) * texelSize).r; 
			depth *= light.farPlane;
			sampleShadow += currentDepth - bias > depth ? 1 : 0;
		}
	}
	sampleShadow /= 9.0;
	return sampleShadow;
}

//todo fix
float CalcShadow(vec4 fragLightSpace, Light light){
	vec3 projCoords = fragLightSpace.xyz / fragLightSpace.w;
	//maps from 0-1
	projCoords = projCoords * 0.5 + 0.5;
	
	vec2 a = vec2(0);
	vec2 b = vec2(1);
	vec2 c = vec2(light.shadowUVs.xy);
	vec2 d = vec2(light.shadowUVs.zw);
	//maps [0,1] onto the shadow map [xy,zw]	
	projCoords.x = map(projCoords.x, a.x,b.x,c.x,d.x);
	projCoords.y = map(projCoords.y, a.y,b.y,c.y,d.y);

	float closestDepth = texture(shadowAtlas, projCoords.xy).r; 
	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	

	//return currentDepth - .05 > closestDepth ? 1 : 0;



	// == Brute Force Shadow Sampler == //
	float bias = .05;
	float sampleShadow = 0.0;
	//seems to work with texture atlas size not defualt map size, wonder if its suppose to do that
	vec2 texelSize = vec2(1.0 / 8096);//textureSize(shadowMap, 0);
	
	
	for(int x = -1; x <= 1; x++) {
		for(int y = -1; y <= 1; y++) {
			float depth = texture(shadowAtlas, projCoords.xy + vec2(x,y) * texelSize).r; 
			sampleShadow += currentDepth - bias > depth ? 1 : 0;
		}
	}
	sampleShadow /= 9.0;
	
	return sampleShadow;
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
	float shadow = CalcShadow(directionalLightLightFragPos, directionalLight );
	
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

	float shadow = CalcPointLightShadows(light);
	//float shadow = CalcCubeShadow(light,vFragPos);

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
	float shadow = CalcShadow(lightFragPos , light);
	
	result = (1.0-shadow) * (diffuse + specular) * light.color; 
	return result;
}