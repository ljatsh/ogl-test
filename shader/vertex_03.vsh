uniform float u_scale;

void main(void)
{
   vec4 a = gl_Vertex; 
   a.x *= u_scale;
   a.y *= u_scale;
   gl_Position = gl_ModelViewProjectionMatrix * a;
}
