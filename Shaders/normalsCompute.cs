#version 430

layout(local_size_x = 16, local_size_y = 32, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D normals_output;
//stuff

vec3 CDM(sampler2D perlin_img, vec2 pixel_coords);

uniform sampler2D perlin_img;
uniform float scale;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	
	float u = pixel_coords.x/512.0f;
	float v = pixel_coords.y/512.0f;
	
	vec2 uv = vec2(u,v);
	
	vec4 perlin = texture(perlin_img, uv);
	vec3 normals = CDM(perlin_img, uv);

    vec4 pixel = vec4(normals.r, normals.g, normals.b, perlin.y);
    imageStore(normals_output, pixel_coords, pixel);
} 

vec3 CDM(sampler2D perlin_img, vec2 pixel_coords)
{
	float right = (textureOffset(perlin_img, pixel_coords, ivec2(1,0)).r) * scale;
    float left = (textureOffset(perlin_img, pixel_coords, ivec2(-1,0)).r) * scale;
    float up = (textureOffset(perlin_img, pixel_coords, ivec2(0,1)).r) * scale;
    float down = (textureOffset(perlin_img, pixel_coords, ivec2(0,-1)).r) * scale;

	float topRight = (textureOffset(perlin_img, pixel_coords, ivec2(1, 1)).r) * scale;
	float topLeft = (textureOffset(perlin_img, pixel_coords, ivec2(-1, 1)).r) * scale;
	float bottomRight = (textureOffset(perlin_img, pixel_coords, ivec2(1, -1)).r) * scale;
	float bottomLeft = (textureOffset(perlin_img, pixel_coords, ivec2(-1, -1)).r) * scale;

	float centre1 = topLeft - bottomRight;
	float centre2 = topRight - bottomLeft;

	float lr = left - right;
    float ud = up - down;

    vec3 normals = normalize(vec3(lr, 2.0, ud));
	vec3 diagnalNormals = normalize(vec3(centre1, 2.0, centre2));
	vec3 normalsCS = normalize(normals + diagnalNormals);
	return normalsCS;
}