#version 330 core

out vec4 FragColor;

in vec2 texCoordsVS ;

uniform sampler2D output_img;

void main()
{
	vec3 img_texture = texture(output_img, texCoordsVS).rgb;
	FragColor = vec4(img_texture, 1.0);
}