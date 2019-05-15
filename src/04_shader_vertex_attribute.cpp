#include "helper.h"
#include "shader.h"

GLuint program_id;

void
init() {
  program_id = load_shaders("shader/vertex_04.vsh", "shader/frag_04.fsh");
  if (program_id != -1) {
    glUseProgram(program_id);
  }
}

void
display() {
  glBegin(GL_TRIANGLES);
      glVertex3f(-0.5f, -0.5f, 0.0f);
      glColor3f(1.0,0.0,0.0);
      glVertex3f( 0.5f, -0.5f, 0.0f);
      glColor3f(0.0,1.0,0.0);
      glVertex3f( 0.5f,  0.5f, 0.0f);
      glColor3f(0.0,0.0,1.0);
  glEnd();
}

int
main() {
  int ret = run(1024, 768, "Tutorial 04 - Shader Vertex Attribute", init, display);

  return ret;
}
