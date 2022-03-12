#version 430

layout(local_size_x = 32, local_size_y = 16, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D normals_output;
//stuff

uniform sampler2D perlin_img;
uniform float scale;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 normal_pixel_coords = vec2(gl_GlobalInvocationID.xy);

	float right = (textureOffset(perlin_img, normal_pixel_coords, ivec2(1,0)).r) * scale;
    float left = (textureOffset(perlin_img, normal_pixel_coords, ivec2(-1,0)).r) * scale;
    float up = (textureOffset(perlin_img, normal_pixel_coords, ivec2(0,1)).r) * scale;
    float down = (textureOffset(perlin_img, normal_pixel_coords, ivec2(0,-1)).r) * scale;
    
    
    float lr = left - right;
    float ud = up - down;
    vec3 normalsCS = normalize(vec3(lr, 2.0, ud));
    vec4 normal_pixel = vec4(normalsCS.x, normalsCS.y, normalsCS.z, 1.0);
    imageStore(normals_output, pixel_coords, normal_pixel);
} 