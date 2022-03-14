#version 430

layout(local_size_x = 16, local_size_y = 32, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D normals_output;
//stuff

vec3 CDM(sampler2D perlin_img, ivec2 pixel_coords);

uniform sampler2D perlin_img;
uniform float scale;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	vec4 perlin = texture(perlin_img, pixel_coords);
	vec3 normals = CDM(perlin_img, pixel_coords);

    vec4 pixel = vec4(normals.r, normals.g, normals.b, perlin.x);
    imageStore(normals_output, pixel_coords, pixel);
} 

vec3 CDM(sampler2D perlin_img, ivec2 pixel_coords)
{
	float right = (textureOffset(perlin_img, pixel_coords, ivec2(1,0)).r) * scale;
    float left = (textureOffset(perlin_img, pixel_coords, ivec2(-1,0)).r) * scale;
    float up = (textureOffset(perlin_img, pixel_coords, ivec2(0,1)).r) * scale;
    float down = (textureOffset(perlin_img, pixel_coords, ivec2(0,-1)).r) * scale;
    
    
    float lr = left - right;
    float ud = up - down;
    vec3 normalsCS = normalize(vec3(lr, 2.0, ud));
	return normalsCS;
}