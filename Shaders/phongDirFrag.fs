#version 330 core

out vec4 FragColor;

in vec2 texCoordsGS;
in vec3 posGS ;
in mat3 gTBN;
in vec3 normalsGS;
in float visibilityGS;
in vec3 flatNormalsGS;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
};                                                                        


struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

uniform DirLight dirLight;
uniform Material mat ;
uniform vec3 viewPos ;
uniform sampler2D normalMap;
uniform int scale;
uniform int Map;
uniform vec3 sky;

void main()
{   
	//colouring by height
    float height = posGS.y/scale;
    vec4 green = vec4(0.3, 0.35, 0.15, 0.0);
    vec4 gray = vec4(0.5, 0.4,0.5, 0.0);
	vec4 red = vec4(1.0, 0.2, 0.2, 0.0);
    
    vec3 colour = vec3(0.1);

    if(height > 0.0 && height < 95.0)
    {
        colour = vec3(mix(green, gray, smoothstep(95.6, 50.0, height)).rgb);
    }
	else if (height >= 95.0)
	{
		colour = vec3(mix(green, red, smoothstep(92.5, 110.0, height)).rgb);//smooth steps may need tuning
	}
    else
    {
        colour = colour;
    } //this will do the colour shading
	
	
	//tri planer for normals
	//percentage multiplier for each axis
	vec3 blendPercent = normalize(abs(normalsGS));
	float b = (blendPercent.x + blendPercent.y + blendPercent.z);
	blendPercent = blendPercent/vec3(b);
	
	//vec3 norm  = texture(normalMap, texCoordsGS * 10).xyz;
	vec3 norm = normalsGS;
	norm = normalize(gTBN*(norm*2.0 - 1.0)) ;//needed for diffuse lighting
	
	//tri planer normal mapping
	vec4 xaxisNormal = texture(normalMap, posGS.yz);
	vec4 yaxisNormal = texture(normalMap, posGS.xz);
	vec4 zaxisNormal = texture(normalMap, posGS.xy);

	vec4 tpTexNorm = xaxisNormal*blendPercent.x + yaxisNormal*blendPercent.y + zaxisNormal*blendPercent.z;
	
	
	vec3 flatNorms = flatNormalsGS;
    vec3 viewDir = normalize(viewPos - posGS);
	
	if(Map == 0)//tri-planer normal mapping
	{
		//Lighting
		vec3 ambient = dirLight.ambient * mat.ambient;     
		vec3 lightDir = normalize(-dirLight.direction);
		// diffuse shading
		float diff = max(dot(norm, -dirLight.direction), 0.0);
		// specular shading
		vec3 reflectDir = reflect(-dirLight.direction, tpTexNorm.rgb);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
		// combine results
	
		vec3 diffuse  = dirLight.diffuse  * (diff * mat.diffuse);
		vec3 specular = dirLight.specular * (spec * mat.specular);
		FragColor = vec4((ambient + diffuse + specular) * colour ,1.0f);
		FragColor = mix(vec4(sky,1.0), FragColor, visibilityGS);//fog
	}
	else if (Map == 1)
	{
		//Lighting
		vec3 ambient = dirLight.ambient * mat.ambient;     
		vec3 lightDir = normalize(-dirLight.direction);
		// diffuse shading
		float diff = max(dot(flatNorms, -dirLight.direction), 0.0);
		// specular shading
		vec3 reflectDir = reflect(-dirLight.direction, flatNorms);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
		// combine results
	
		vec3 diffuse  = dirLight.diffuse  * (diff * mat.diffuse);
		vec3 specular = dirLight.specular * (spec * mat.specular);
		FragColor = vec4((ambient + diffuse + specular) * colour ,1.0f);
		FragColor = mix(vec4(sky,1.0), FragColor, visibilityGS);//fog
	}
	

}

