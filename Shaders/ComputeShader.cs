#version 430

layout(local_size_x = 16, local_size_y = 32) in;
layout (binding = 0, rgba32f) uniform image2D img_output;
//stuff

void main()
{
	vec4 pixel = vec4(1.0, 0.5, 0.0, 1.0);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	imageStore(img_output, pixel_coords, pixel);
} 