#version 330 core
//out vec4 FragColor;

layout (location = 0) out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, texCoords);
}