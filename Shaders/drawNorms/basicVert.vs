#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBiTan;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vNorm;
out vec2 vUV;
out mat3 TBN;
out vec3 posWS;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	vNorm = aNorm;
	vUV = aUV;
	
	vec3 T = (model*vec4(aTan,0.0)).xyz;
    vec3 B = (model*vec4(aBiTan,0.0)).xyz;
    TBN = mat3(T, B, vNorm);//moves to tangent space
	
	posWS = (model*vec4(aPos,1.0)).xyz;
	
}