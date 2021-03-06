#version 330 core
layout(triangles) in ;
layout(triangle_strip, max_vertices = 3) out ;

in vec3 posES[] ;
in vec2 TexCoordsES[];
in float visibilityES[];

out vec3 posGS ;
out vec3 flatNormalsGS;
out vec2 TexCoordsGS;
out float visibilityGS;

vec3 GetSurfaceNormal();

void main()
{
   flatNormalsGS = GetSurfaceNormal();
   for(int i = 0 ; i < 3; i++)
   {
      gl_Position = gl_in[i].gl_Position ;
      posGS = posES[i] ;  
      TexCoordsGS = TexCoordsES[i];
      visibilityGS = visibilityES[i];
      EmitVertex() ;
  }
     EndPrimitive() ;
}

vec3 GetSurfaceNormal(){
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
}