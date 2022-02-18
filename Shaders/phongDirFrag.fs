#version 330 core

out vec4 FragColor;

in vec2 texCoordsGS;
in vec3 posGS ;
in mat3 gTBN;
in vec3 normalsGS;


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


uniform int scale;

void main()
{   
    vec3 norm  = texture(normalMap, texCoordsGS).xyz;
	norm = normalize(gTBN*(norm*2.0 - 1.0)) ;
	//vec3 norm = normalsGS; //will swap to flat shading, not quite right however
    vec3 viewDir = normalize(viewPos - posGS);
	
    float height = posGS.y/scale;
    vec4 green = vec4(0.3, 0.35, 0.15, 0.0);
    vec4 gray = vec4(0.5, 0.4,0.5, 0.0);
    
    /*vec3 colour = vec3(0.1);

    if(height > 0.00005)
    {
        colour = vec3(mix(green, gray, smoothstep(0.1, 1.0, height)).rgb);
    }
    else
    {
        colour = colour;
    }*/ //this will do the colour shading from the video


	vec3 ambient = dirLight.ambient * mat.ambient;     
    vec3 lightDir = normalize(-dirLight.direction);
    // diffuse shading
    float diff = max(dot(norm, -dirLight.direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-dirLight.direction, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    // combine results
   
    vec3 diffuse  = dirLight.diffuse  * (diff * mat.diffuse);
    vec3 specular = dirLight.specular * (spec * mat.specular);
    FragColor = vec4((ambient + diffuse + specular),1.0f);

    //FragColor = vec4(colour, 1.0);//swaps the result to the coloured version from the video
}

