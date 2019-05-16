#include "helper.h"
#include "shader.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 1. The compiler is allowed to optimize alway any unused uniforms. So glGetUniformLocation may return -1
//    https://stackoverflow.com/questions/24240107/glsl-cant-get-uniforms-location
//    https://www.khronos.org/opengl/wiki/GLSL_:_common_mistakes
// 2. glUniform should be called after glUseProgram. Otherwise, GL_INVALID_OPERATION was generated

GLuint program_id;
GLuint u_scale;
GLuint u_color;

void
init() {
  program_id = load_shaders("shader/vertex_03.vsh", "shader/frag_03.fsh");
  if (program_id != -1) {
    glUseProgram(program_id);

    u_scale = glGetUniformLocation(program_id, "u_scale");
    check_gl_state();

    if (u_scale == -1)
      fprintf(stderr, "failed to get uniform u_scale\n");
    else {
      GLfloat scale;
      glGetUniformfv(program_id, u_scale, &scale);
      check_gl_state();
      fprintf(stdout, "origin u_scale: %.1f\n", scale);

      glUniform1f(u_scale, 0.5);
      check_gl_state();

      glGetUniformfv(program_id, u_scale, &scale);
      check_gl_state();
      fprintf(stdout, "reset u_scale: %.1f\n", scale);
    }

    u_color = glGetUniformLocation(program_id, "u_color");
    if (u_color == -1)
      fprintf(stderr, "failed to get uniform u_color\n");
    else {
      GLfloat colors[4];
      glGetUniformfv(program_id, u_color, colors);
      check_gl_state();
      fprintf(stdout, "origin color: %.1f, %.1f, %.1f, %.1f\n", colors[1], colors[2], colors[3], colors[4]);

      glUniform4f(u_color, 1.0, 1.0, 0.0, 1.0);
      check_gl_state();

      glGetUniformfv(program_id, u_color, colors);
      check_gl_state();
      fprintf(stdout, "reset color: %.1f, %.1f, %.1f, %.1f\n", colors[1], colors[2], colors[3], colors[4]);
    }
  }
}

void
display() {
  draw_solid_teapot(1.0);
}

int
main() {
  int ret = run(1024, 768, "Tutorial 03 - Shader Uniform Variable", init, display);

  return ret;
}
