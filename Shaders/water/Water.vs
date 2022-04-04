#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aUV;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoordsVS;
out vec3 posVS ;
out vec3 normVS;

void main()
{
    texCoordsVS = aUV;
    posVS = (model * vec4(aPos, 1.0)).xyz; 
    
    gl_Position = projection * view * model*vec4(aPos,1.0);
    normVS = aNormals ;
}