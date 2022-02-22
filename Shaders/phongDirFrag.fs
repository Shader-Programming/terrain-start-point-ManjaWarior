#version 330 core

out vec4 FragColor;

in vec2 texCoordsGS;
in vec3 posGS ;
in mat3 gTBN;
in vec3 normalsGS;
in float visibilityGS;


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

//uniform sampler2D texture1;
uniform DirLight dirLight;
uniform Material mat ;
uniform vec3 viewPos ;

uniform sampler2D normalMap;
uniform sampler2D colourMap;

uniform int scale;

uniform vec3 sky;

void main()
{   
	//colouring by height
    float height = posGS.y/scale;
    vec4 green = vec4(0.3, 0.35, 0.15, 0.0);
    vec4 gray = vec4(0.5, 0.4,0.5, 0.0);
    
    vec3 colour = vec3(0.1);

    if(height > 0.00005)
    {
        colour = vec3(mix(green, gray, smoothstep(0.1, 1.0, height)).rgb);
    }
    else
    {
        colour = colour;
    } //this will do the colour shading from the video
	FragColor = vec4(colour, 1.0);//swaps the result to the coloured version from the video
	
	
	//tri planer
	
	//percentage multiplier for each axis
	vec3 blendPercent = normalize(abs(normalsGS));
	float b = (blendPercent.x + blendPercent.y + blendPercent.z);
	blendPercent = blendPercent/vec3(b);
	
	//now blend the three values with the texture from each axis perspective
	vec4 xaxis = texture(colourMap, posGS.yz);
	vec4 yaxis = texture(colourMap, posGS.xz);
	vec4 zaxis = texture(colourMap, posGS.xy);
	
	vec4 tpTex = xaxis*blendPercent.x + yaxis*blendPercent.y + zaxis*blendPercent.z;
	
	
	vec3 norm  = texture(normalMap, texCoordsGS).xyz;
	norm = normalize(gTBN*(norm*2.0 - 1.0)) ;
	
	//tri planer normal mapping
	vec4 xaxisNormal = texture(normalMap, posGS.yz);
	vec4 yaxisNormal = texture(normalMap, posGS.xz);
	vec4 zaxisNormal = texture(normalMap, posGS.xy);

	vec4 tpTexNorm = xaxisNormal*blendPercent.x + yaxisNormal*blendPercent.y + zaxisNormal*blendPercent.z;
	
	
	//vec3 norm = normalsGS; //will swap to flat shading, not quite right however
	
	
    vec3 viewDir = normalize(viewPos - posGS);
	
	
	//Lighting
	vec3 ambient = dirLight.ambient * mat.ambient /** tpTex.rgb*/;     
    vec3 lightDir = normalize(-dirLight.direction);
    // diffuse shading
    float diff = max(dot(norm /*tpTexNorm.rgb*/, -dirLight.direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-dirLight.direction, norm /*tpTexNorm.rgb*/);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    // combine results
   
    vec3 diffuse  = dirLight.diffuse  * (diff * mat.diffuse) /* * tpTex.rgb*/;
    vec3 specular = dirLight.specular * (spec * mat.specular);
    //FragColor = vec4((ambient + diffuse + specular),1.0f);
	//FragColor = mix(vec4(sky,1.0), FragColor, visibilityGS);

}

