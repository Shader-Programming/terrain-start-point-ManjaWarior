#version 410 core

out vec4 FragColor;

in vec3 flatNormalsGS;
in vec3 posGS;
in vec2 TexCoordsGS;
in float visibilityGS;

uniform sampler2D normalMap;
uniform sampler2D DuDvMap;
uniform sampler2D reflection;
uniform sampler2D refraction;
uniform float time;
uniform vec3 lightDir;

uniform float screenW;
uniform float screenH;
uniform vec3 viewPos;
uniform bool flatShadingOn;

vec3 highlights(vec3 norms, vec3 viewDir);

float waveStrength = 0.04;
vec3 lightColour = vec3(1.0, 1.0, 1.0);

void main()
{
    vec3 viewDir = normalize(viewPos - posGS);

    vec2 scaleUV = vec2(TexCoordsGS) * 2;
    vec2 differential = (texture(DuDvMap, vec2(scaleUV.x + (time * 0.02), scaleUV.y)).rg * 2.0 - 1.0) * waveStrength;
    vec2 reflectTex = vec2((gl_FragCoord.x) / screenW, (-gl_FragCoord.y) / screenH);
    vec2 refractTex = vec2((gl_FragCoord.x) / screenW, (gl_FragCoord.y) / screenH);

	vec3 normals = vec3(1.0);	
	vec3 waterNormals = texture(normalMap, differential).xyz;
	
	if(!flatShadingOn) 
    {
        normals = normalize(vec3(waterNormals.r * 2.0 - 1.0, waterNormals.b, waterNormals.g * 2.0 - 1.0));
	}
    else 
    {
        normals = flatNormalsGS;
    }

    vec4 reflect = texture(reflection, reflectTex + differential);
    vec4 refract = texture(refraction, refractTex + differential);

    float fresnel = dot(viewDir, vec3(0, 1, 0));
    vec4 result = vec4(vec3(mix(reflect, refract, fresnel)), 1.0) + vec4(highlights(normals, viewDir), 1.0);
    FragColor = result;
}

vec3 highlights(vec3 norms, vec3 viewDir)
{
    vec3 halfwaydir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norms, halfwaydir), 0.0), 1) * 0.25;
    vec3 specular = spec * lightColour;
    return specular;
}