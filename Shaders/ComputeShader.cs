#version 430

layout(local_size_x = 16, local_size_y = 32) in;
layout (binding = 0, rgba32f) uniform image2D img_output;
//stuff

float noise(vec3 position, int octaves);
float snoise(vec3 x);
float hash(float n);

uniform int scale;
uniform int octaves;

void main()
{
	vec4 pixel = vec4(1.0, 0.5, 0.0, 1.0);
	float local_y = noise(gl_GlobalInvocationID.xyz, octaves);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.x, local_y);
	imageStore(img_output, pixel_coords, pixel);
} 


float hash(float n) {
    return fract(sin(n) * 753.5453123);
}

float snoise(vec3 x)
{
	vec3 p = floor(x);
	vec3 f = fract(x);
	f = f * f * (3.0f - (2.0f * f));

	float n = p.x + p.y * 157.0f + 113.0f * p.z;
	return mix(mix(mix(hash(n + 0.0f), hash(n + 1.0f), f.x),
		mix(hash(n + 157.0f), hash(n + 158.0f), f.x), f.y),
		mix(mix(hash(n + 113.0f), hash(n + 114.0f), f.x),
			mix(hash(n + 270.0f), hash(n + 271.0f), f.x), f.y), f.z);
}

float noise(vec3 position, int octaves)
{
	float total = 0.0f;
	float frequency = 0.005f;
	float maxAmplitude = 0.0f;
	float amplitude = 50.0f;
	for(int i = 0; i < octaves; i++)
	{
		total += snoise(position * frequency) * amplitude;
		frequency *= 2.0f;
		amplitude /= 2.0f;
		maxAmplitude += amplitude;
	}
	return (total / maxAmplitude) * scale;
}