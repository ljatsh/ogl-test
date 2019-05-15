void main(void)
{
   vec4 a = gl_Vertex + gl_Color;           // misuse of color
   gl_Position = gl_ModelViewProjectionMatrix * a;
}