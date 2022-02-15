#version 330 core
layout(triangles) in ;
layout(triangle_strip, max_vertices = 3) out ;
vec3 getNormal() ;

vec3 calcBiTangent() ;
vec3 calcTangent() ;

in vec3 posES[] ;
in vec2 texCoordsES[] ;
in vec3 normalsES[] ;

out vec2 texCoordsGS ;
out vec3 gWorldPos_FS_in ;
out mat3 gTBN;

void main()
{
   vec3 tangent = calcTangent();
   vec3 biTangent = calcBiTangent();
   
   for(int i = 0 ; i < 3; i++)
   {
      gl_Position = gl_in[i].gl_Position ;
      gWorldPos_FS_in = posES[i] ;
      //texCoordsGS = getNormal() ;  
	  
	  gTBN = mat3(tangent, biTangent, normalsES[i]);
	  
      EmitVertex() ;
  }
     EndPrimitive() ;

}


/*vec3 getNormal()
{
    vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
    return normalize(cross(a, b));
}*/

vec3 calcTangent()
{
    vec3 deltaPos1 = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 deltaPos2 = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
	
	vec2 deltaUV1 = texCoordsES[1] - texCoordsES[0];
	vec2 deltaUV2 = texCoordsES[2] - texCoordsES[0];
	
	float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	
	vec3 thisTangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	vec3 thisBiTangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
	
	//mat3 TBN = mat3(normalize(thisTangent), normalize(thisBiTangent), normalsES[0])
	
    return normalize(thisTangent);
}

vec3 calcBiTangent()
{
    vec3 deltaPos1 = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 deltaPos2 = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
	
	vec2 deltaUV1 = texCoordsES[1] - texCoordsES[0];
	vec2 deltaUV2 = texCoordsES[2] - texCoordsES[0];
	
	float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	
	vec3 thisTangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	vec3 thisBiTangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
	
	//mat3 TBN = mat3(normalize(thisTangent), normalize(thisBiTangent), normalsES[0])
	
    return normalize(thisBiTangent);
}