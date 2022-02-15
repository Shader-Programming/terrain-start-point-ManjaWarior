#version 330 core

layout (triangles) in;//name in brackets must be primitve type input, includes points, lines, triangles, lines_adjacency, traingles_adjacency
layout(line_strip, max_vertices = 2) out;//outputs the primitve type and the number of vertices, points, line_strip, triangle_strip

uniform float normLength;

in vec3 vNorm[];

out mat3 TBN;

void main()
{
	for(int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = (gl_in[i].gl_Position+vec4(vNorm[i]*normLength,0.0)); //normLength is a uniform
		EmitVertex();//this draws the normals when primitive type is line_strip
	}
	
	EndPrimitive();//Finish the primitive
}