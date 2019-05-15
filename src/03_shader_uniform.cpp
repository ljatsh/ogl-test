#include "helper.h"
#include "shader.h"

GLuint program_id;
GLuint u_scale;
GLuint u_color;

void
init() {
  program_id = load_shaders("shader/vertex_03.vsh", "shader/frag_03.fsh");
  if (program_id != -1) {
    u_scale = glGetUniformLocation(program_id, "u_scale");
    u_scale = glGetUniformLocation(program_id, "u_color");

    if (u_scale == -1)
      fprintf(stderr, "failed to get uniform u_scale\n");
    else {
      glUniform1f(u_scale, 1.0);
      glUniform4f(u_color, 1.0, 0.0, 0.0, 1.0);
    }

    glUseProgram(program_id);
  }
}

void
display() {
  draw_wire_teapot(1.0);
}

int
main() {
  int ret = run(1024, 768, "Tutorial 03 - Shader Uniform Variable", init, display);

  return ret;
}
