#version 410 core

out vec4 FragColor;

vec3 getDirectionalLight(vec3 norm, vec3 viewDir);

uniform sampler2D texture_diffuse1;//pulls the diffuse map through from Mesh class
uniform sampler2D texture_normal1;//pulls the normal map from the mesh class
uniform sampler2D texture_specular1;//pulls the specular map from the mesh class

uniform vec3 lightCol;
uniform vec3 lightDir;
uniform vec3 viewPos;

in vec3 gNorm;
in vec2 gUV;
in mat3 gTBN;
in vec3 gposWS;

float shine = 128;
float ambientFactor = 0.3f;


void main()
{    
    vec3 norm = normalize(gNorm);//surface normal values passed in from the geometry shader

	//vec3 norm = gNorm;
	//norm = texture(texture_normal1, gUV).xyz;
	//norm = normalize(gTBN*(gNorm*2.0 - 1.0)); //this is the normal mapping values for lighting
	
	vec3 viewDir = normalize(viewPos - gposWS);
	
	vec3 result = vec3(0.0f);
	result = getDirectionalLight(norm, viewDir);

	FragColor = vec4(result, 1.0f);
	
	//FragColor = texture(texture_diffuse1, gUV);
	//FragColor = vec4(0.5,0.5,0.5,1.0f) ;
}



vec3 getDirectionalLight(vec3 norm, vec3 viewDir)
{
    vec3 diffMapColor = texture(texture_diffuse1, gUV).rgb;
    float specMapColor = texture(texture_specular1, gUV).r;
    //ambient light
    vec3 ambientColor = lightCol * diffMapColor * ambientFactor;

    //diffuse light
    float diffuseFactor = dot(norm, -lightDir);
    diffuseFactor = max(diffuseFactor,0.0);
    vec3 diffuseColor = lightCol * diffMapColor * diffuseFactor;

    //specular light 
    vec3 halfwayDir = normalize(viewDir - lightDir);
    float specularFactor = dot(norm, halfwayDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = lightCol * specularFactor * specMapColor ;

    vec3 result = ambientColor + diffuseColor + specularColor;
    return result;
}