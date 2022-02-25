#version 330 core

layout (triangles) in;//name in brackets must be primitve type input, includes points, lines, triangles, lines_adjacency, traingles_adjacency
layout(triangle_strip, max_vertices = 3) out;//outputs the primitve type and the number of vertices, points, line_strip, triangle_strip

vec3 getSurfaceNormal();
vec3 calcBiTangent() ;
vec3 calcTangent() ;

in vec3 normalsES[];
in vec2 texCoordsES[];
in vec3 posES[];//arrays for each triangle in the mesh
in float visibilityES[];


out vec3 normalsGS;
out vec2 texCoordsGS;
out mat3 gTBN;
out vec3 posGS;
out float visibilityGS;

void main()
{
	//normalsGS = getSurfaceNormal();//the surface normal per triangle strip
	for(int i = 0; i < 3; i++)
	{
		texCoordsGS = texCoordsES[i];
		posGS = posES[i];
		normalsGS = normalsES[i];
		gl_Position = gl_in[i].gl_Position;
		
		visibilityGS = visibilityES[i];
		
		gTBN = mat3(calcTangent(), calcBiTangent(), normalize(normalsES[i]));
		EmitVertex();
	}
	

	EndPrimitive();
}

vec3 getSurfaceNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

vec3 calcTangent()
{
    vec3 deltaPos1 = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 deltaPos2 = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
	
	vec2 deltaUV1 = texCoordsES[1] - texCoordsES[0];
	vec2 deltaUV2 = texCoordsES[2] - texCoordsES[0];
	
	float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	
	vec3 thisTangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	
	
    return normalize(thisTangent);
}

vec3 calcBiTangent()
{
    vec3 deltaPos1 = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 deltaPos2 = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
	
	vec2 deltaUV1 = texCoordsES[1] - texCoordsES[0];
	vec2 deltaUV2 = texCoordsES[2] - texCoordsES[0];
	
	float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	
	vec3 thisBiTangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
	
	
    return normalize(thisBiTangent);
}