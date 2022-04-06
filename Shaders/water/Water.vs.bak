#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aUV;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

out vec2 texCoordsVS;
out vec3 posVS ;
out vec3 normVS;

struct Wave
{
	float amp;
	vec2 waveDir;
	float crestdist;
	float speed;
};

#define numWaves 4
uniform Wave waves[numWaves];

vec3 CalcGerstnerWaves(vec3 V, float amp, vec2 wavedir, float crestdist, float speed);

void main()
{
    texCoordsVS = aUV;
    posVS = (model * vec4(aPos, 1.0)).xyz; 
    
	vec3 positionModification = vec3(0.0f);
	for(int i = 0; i < numWaves; i++)
	{
		posVS  = CalcGerstnerWaves(posVS, waves[i].amp, waves[i].waveDir, waves[i].crestdist, waves[i].speed);
	}
	
    gl_Position = projection * view * model * vec4(posVS, 1.0);
    normVS = aNormals ;
}

vec3 CalcGerstnerWaves(vec3 V, float amp, vec2 wavedir, float crestdist, float speed){//calculates the waves movement to modify position 
    speed*=time;
    float wavelength = (2*3.14152)/crestdist;
    float phasespd = sqrt(9.8)/wavelength; //phase speed

    float f = V.y+(dot(V.xy,wavedir)-phasespd*speed)*wavelength;
    V.x += wavedir.x*(amp*cos(f));
    V.y += amp*sin(f);
    V.z += wavedir.y*(amp*cos(f));

    return V;
}